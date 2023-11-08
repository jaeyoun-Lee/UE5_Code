// Fill out your copyright notice in the Description page of Project Settings.


#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "GBAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "NiagaraComponent.h"
#include "GBHUDWidget.h"
#include "NextLevelWidget.h"
#include "CableComponent.h"			// ���̺� ������Ʈ�� ���� ��� ����
#include "GBGameState.h"

// Sets default values
AGBCharacter::AGBCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	aimSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Aim Spring Arm"));
	aimCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Aim Camera"));
	cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	shockLightning = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShockLightning"));

	springArm->SetupAttachment(RootComponent);
	camComp->SetupAttachment(springArm);

	aimSpringArm->SetupAttachment(RootComponent);
	aimCamComp->SetupAttachment(aimSpringArm);
	cable->SetupAttachment(RootComponent);
	shockLightning->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKallari/Characters/Heroes/Kallari/Skins/RedDeath/Meshes/Kallari_RedDeath.Kallari_RedDeath'"));

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> animation(TEXT("/Script/Engine.AnimBlueprint'/Game/BP/Animation/ABP_GBCharacter.ABP_GBCharacter_C'"));
	if (animation.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animation.Class);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempShock(TEXT("/Script/Niagara.NiagaraSystem'/Game/BP/FX/shock/P_Shock.P_Shock'"));
	if (tempShock.Succeeded())
	{
		shockLightning->SetAsset(tempShock.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/PlayerSound/WireSFX_Cue.WireSFX_Cue'"));
	if (tempSound.Succeeded())
	{
		grapSound = tempSound.Object;
;	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tempSFX(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/PlayerSound/Hit-the-Attack_Cue.Hit-the-Attack_Cue'"));
	if (tempSFX.Succeeded())
	{
		beAttackSFX = tempSFX.Object;
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 5.19f, -95.0f), FRotator(0, -90.0f, 0));
	GetCapsuleComponent()->SetCapsuleHalfHeight(93.762238f);	// ĸ�� ������Ʈ ���� ����
	GetCapsuleComponent()->SetCapsuleRadius(33.495377f);		// ĸ�� ������Ʈ �ݰ� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));		// Player �ݸ��� ������ ���

	CameraSetting();	// ī�޶� �� ĳ���� ȸ�� ������ ���� �Լ�
	animMontageSetting();

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;	// ĳ���� �ӵ� ����
	GetCharacterMovement()->JumpZVelocity = 650.0f;		// ĳ���� ���� ���� ����
	
	canAttacking = true;	// �÷��̾ Enemy���� �¾Ƶ� �Ǵ� �������� Ȯ�����ִ� ����(���� ������ �޴� ��, �ٸ� ������ ���� ���ϰ��ϱ� ����)
	IsAttacking = false;	// ���� ������ Ȯ�����ִ� ����
	isJumping = false;		// Space�ٸ� ���� ������ ������ 
	isGuarding = false;		// ���� ������ Ȯ�����ִ� ����
	isDeath = false;
	IsShocking = false;		// ���� ������ �¾Ҵ��� Ȯ�����ִ� ����
	isComboInputOn = false;
	attackRange = 150.0f;
	attackRadius = 70.0f;

	if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleOutSide_Modify")))
	{
		max_HP = 400.0f;
	}

	else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleInSide")))
	{
		max_HP = 250.0f;
	}

	else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("BossStage")))
	{
		max_HP = 200.0f;
	}

	current_HP = max_HP;

	cable->SetVisibility(false);
	cable->CableWidth = 3.0f;		// ���̺� ������Ʈ�� �β� 3cm�� ����

	shockLightning->SetVisibility(false);
}


void AGBCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GBAnim = Cast<UGBAnimInstance>(GetMesh()->GetAnimInstance());
	GBCHECK(GBAnim != nullptr);

	// bNextAttack ��Ƽ���̸� ������ UGBAnimInstance::AnimNotify_bNextAttack()�� ȣ��ǰ�,
	// UGBAnimInstance::AnimNotify_bNextAttack() �Լ����� OnNextAttackCheck ��������Ʈ ȣ��
	GBAnim->OnNextAttackCheck.AddLambda([this]() -> void {
			OnNextAttack();
		});

	// ResetCombo ��Ƽ���̸� ������ UGBAnimInstance::AnimNotify_ResetCombo�� ȣ��ǰ�,
	// UGBAnimInstance::AnimNotify_ResetCombo �Լ����� OnResetCheck ��������Ʈ ȣ��
	// �޺� ������ �Էµ��� �ʾҰų�, �ִ� �޺��� �������� ȣ��Ǵ� ��������Ʈ
	GBAnim->OnResetCheck.AddLambda([this]() -> void {
			GBLOG(Warning, TEXT("Called by OnResetCheck"));
			OnResetAttack();
		});

	// ���� �ִ� ��Ÿ�ֿ��� ������ �¾Ҵ��� Ȯ���Ҷ� ȣ��Ǵ� ��Ƽ����
	GBAnim->OnAttackHitCheck.AddUObject(this, &AGBCharacter::AttackCheck);		

	// ���� ������ �°�, ������ ������ ȣ��Ǵ� �Լ�
	GBAnim->OnShockCheck.AddUObject(this, &AGBCharacter::OnEndShock);

	// �÷��̾� ��� ��, ȣ��Ǵ� ����
	GBAnim->OnPlayerDead.AddLambda([this]() -> void {
		GBPlayerController->ShowFailedUI();
	});
}


// Called when the game starts or when spawned
void AGBCharacter::BeginPlay()
{
	Super::BeginPlay();

	GBPlayerController = Cast<AGBPlayerController>(GetController());
	GBCHECK(GBPlayerController != nullptr);

	GBPlayerController->GetHUDWidget()->UpdatePlayerHP(GetHPRatio());

	GBPlayerController->GetHUDWidget()->OnPlayerDie.AddLambda([this]() -> void {

		GBLOG(Error, TEXT("Player is Dead....!! Game Over"));

		// SetActorEnableCollision(false);			// �浹 ���� ����
		PlayAnimMontage(deathMontage, 0.7f);	// ��� �ִϸ��̼� ���
		DisableInput(GBPlayerController);		// �ԷºҰ�
		isDeath = true;
		});

	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));	// ���� ������Ʈ ��������
	if (gameState)
	{
		GBPlayerController->GetHUDWidget()->UpdateEnemyCnt(gameState->GetEnemyCount());
	}

	if (gameState != nullptr)
	{
		// ���� ������ ó�� ��, 3�� �ڿ� GameClear() �Լ� ȣ��
		gameState->OnGameClear.AddLambda([this]() -> void {
			GetWorld()->GetTimerManager().SetTimer(clearUIStartTimers, this, &AGBCharacter::GameClear, 1.0f, false, 3.0f);
		});
	}
}

// Called every frame
void AGBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GBAnim->isGrappling)
	{
		FVector modify_grapPoint = grabPoint;
		modify_grapPoint.Z -= 60.0f;

		cable->EndLocation = GetActorTransform().InverseTransformPosition(modify_grapPoint);
		FVector force1 = GetUnitDirection(GetActorLocation(), grabPoint);
		FVector force2 = GetActorRightVector() * float(GetInputAxisValue(TEXT("LeftRight"))) * 0.7;
		auto total = force1 + force2;
		total.Normalize(0.0001);

		total = total * float(200000);

		GetCharacterMovement()->AddForce(total);		// ������ �� �������� ���ư�
	}
}

// Called to bind functionality to input
void AGBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AGBCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AGBCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AGBCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AGBCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AGBCharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AGBCharacter::Run);	// ������ ������ �޸��� ��� On
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AGBCharacter::Walk);	// ���� �޸��� ��� Off

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AGBCharacter::Attack);	
	PlayerInputComponent->BindAction(TEXT("Shield"), IE_Pressed, this, &AGBCharacter::Shield);
	PlayerInputComponent->BindAction(TEXT("Shield"), IE_Released, this, &AGBCharacter::ShieldEnd);

	PlayerInputComponent->BindAction(TEXT("Grappling"), IE_Pressed, this, &AGBCharacter::Grappling);
	PlayerInputComponent->BindAction(TEXT("Grappling"), IE_Released, this, &AGBCharacter::GrapplingEnd);
}

void AGBCharacter::Attack()
{
	if (!GBAnim->isAir && !GBAnim->isGrappling)		// ���� �����鼭,
	{
		if (IsAttacking)		// ���� ���̸�
		{
			isComboInputOn = true;
		}

		else                   // ù �����̸�
		{
			IsAttacking = true;
			currentCombo = 1;
			PlayAnimMontage(AttackMontage_A);
		}
	}

	else     // ���߿� �����鼭
	{
		if (!IsAttacking)	// ù �����̸�(���߿��� ���� 1���� �� �� �ֵ���)
		{
			GrapplingEnd();			// ���̾� Ÿ�� ���󰡸鼭 ���� �ϸ�, ���̾� ����������
			attackRadius = 85.0f;
			IsAttacking = true;
			PlayAnimMontage(AttackAirMontage);
			GBAnim->bAirAttack = true;
		}
	}
}


void AGBCharacter::AttackCheck()
{
	FHitResult hitResult;									// ������ �浹�� Ž���� ���, ���õ� ������ ���� ����ü
	FCollisionQueryParams params(NAME_None, false, this);	// Ž�� ����� ���� ���� ���� ��Ƶ� ����ü

	bool bResult = GetWorld()->SweepSingleByChannel(	// Trace ä�η� ������ �浹 ���� Ȯ��
		hitResult,													
		GetActorLocation(),											// ���� ĳ���� ��ġ���� �����Ͽ�
		GetActorLocation() + GetActorForwardVector() * attackRange,	// ���� ĳ���� ���� 200cm Ž��
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,					// Attack Ʈ���̽� ���
		FCollisionShape::MakeSphere(attackRadius),					// ������ 50cm�� ��ü�� Ž���Ѵ�.
		params);

#if ENABLE_DRAW_DEBUG	// ����� ������� �����ϰ� �ϱ����� �ڵ�
	
	FVector traceVec = GetActorForwardVector() * attackRange;		// ĳ������ �ü����� ����
	FVector center = GetActorLocation() + traceVec * 0.5f;			// ĳ���� ���� ��ġ���� ���� ���� ��ġ
	float halfHeight = attackRange * 0.5f + attackRadius;			// ĸ�� ���� ����
	FQuat capsuleRot = FRotationMatrix::MakeFromZ(traceVec).ToQuat();	// ĸ���� �׸������� �ʿ��� ȸ�� ��ǥ�� 
	FColor drawColor = bResult ? FColor::Green : FColor::Red;			// ���� ���� ��, �ʷ���, ���� ���� ��, �������� ����� ��ο�
	float debugLifeTime = 5.0f;											// ����� ��ο�(ĸ�� ���) ���� �ð�

	DrawDebugCapsule(GetWorld(),
		center,
		halfHeight,
		attackRadius,
		capsuleRot,
		drawColor,
		false,
		debugLifeTime);

#endif		// ~#if ENABLE_DRAW_DEBUG  ����� ������� �����ϰ� �ϱ����� �ڵ�

	if (bResult)	// �浹 �����Ǹ�(���ݿ� ������)
	{
		if (hitResult.GetActor())	// Ž���� ���͸� ������
		{
			UGameplayStatics::PlaySound2D(GetWorld(), beAttackSFX);

			GBLOG(Warning, TEXT("Hit Actor Name : %s"), *hitResult.GetActor()->GetName());
			float randDamage = 35 * (FMath::RandRange(0.5, 1.2));		// ���� ������ �������� ����

			FDamageEvent DamageEvent;		// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
			hitResult.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()�� ���� ���� ���͸� �ǹ��Ѵ�.
		}
	}
}

// bNextAttack ��Ƽ���̸� ������ ��������Ʈ���� ȣ���ϴ� �Լ�
void AGBCharacter::OnNextAttack()
{
	GBLOG(Warning, TEXT("Called by OnNextAttackCheck"));
	GBCHECK(!GBAnim->isAir);
	// �޺� ������ �ԷµǾ�����
	if (isComboInputOn)		
	{
		isComboInputOn = false;
		if (currentCombo == 1)
		{
			currentCombo = 2;
			currentMontage = AttackMontage_B;
		}

		else if (currentCombo == 2)
		{
			currentCombo = 3;
			currentMontage = AttackMontage_C;
		}

		else if (currentCombo == 3)
		{
			currentCombo = 0;
			currentMontage = AttackMontage_D;
		}
		PlayAnimMontage(currentMontage);
	}
}

// ResetCombo ��Ƽ���̸� ������ ��������Ʈ���� ȣ���ϴ� �Լ�
void AGBCharacter::OnResetAttack()
{
	currentCombo = 0;
	isComboInputOn = false;
	IsAttacking = false;
	attackRadius = 70.0f;
}


// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
float AGBCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (canAttacking)
	{
		GBLOG(Error, TEXT("Character TakeDamage"));
		float finalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		if (isGuarding)
		{
			finalDamage *= 0.2;		// ���� �����̸� ������� �� ��������
		}

		GBLOG(Warning, TEXT("Actor : %s, took Damage : %f"), *GetName(), finalDamage);
		current_HP -= finalDamage;			// ���� ü�¿��� ���� ���ظ� ���ش�.
		isGuarding = false;					// ���� ��������, ���� ����

		if (IsShocking)		// ���� ���ݿ� �������,
		{
			canAttacking = false;
			PlayAnimMontage(shockMontage, 1.0f);
			shockLightning->SetVisibility(true);
			DisableInput(GBPlayerController);		// ���Ͽ� �ɸ� ���·� ����� ���ؼ� �ԷºҰ�
		}

		else               // �ٸ� ���ݿ� �������,
		{
			PlayAnimMontage(hitMontage, 1.0f);
		}

		//  ����� ü���� UI�� �������ִ� UpdatePlayerHP() �Լ� ȣ��
		GBPlayerController->GetHUDWidget()->UpdatePlayerHP(GetHPRatio());

		return finalDamage;
	}

	else
		return 0;
}

// ���� ������ �°�, ������ ������ ȣ��Ǵ� �Լ�
void AGBCharacter::OnEndShock()
{
	canAttacking = true;
	IsShocking = false;
	shockLightning->SetVisibility(false);
	StopAnimMontage(shockMontage);
	EnableInput(GBPlayerController);		// ���� ���� ���� = �Է°���
}

void AGBCharacter::GameClear()
{
	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));

	if (gameState != nullptr)
	{
		if (GBPlayerController != nullptr)
		{
			GBPlayerController->ShowSucceededUI();
			GetCharacterMovement()->MaxWalkSpeed = 0;		// ���� Clear UI�� ������ ��, �޸��� ���̸� ��� �޸���. �̸� ���� ����.
			GrapplingEnd();									// ���� Clear UI�� ������ �� �Ŵ޷��ִ°� �����ϱ� ���ؼ�

			GetWorld()->GetTimerManager().SetTimer(creditStartTimers, this, &AGBCharacter::EndingCredit, 1.0f, false, 2.5f);
		}
	}
}

void AGBCharacter::EndingCredit()
{
	if (GBPlayerController != nullptr)
	{
		GBPlayerController->ShowNextUI();				// ȭ���� �帴������ UI ����
		GBPlayerController->bShowMouseCursor = false;	// ���콺 Ŀ�� X

		auto nextWidget = Cast<UNextLevelWidget>(GBPlayerController->GetNextLevelWidget());
		if (nextWidget != nullptr)
		{
			UWidgetAnimation* opacityAnim = nextWidget->GetWidgetAnimation();
			if (opacityAnim != nullptr)
			{
				nextWidget->PlayAnimation(opacityAnim);
			}
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(creditStartTimers);
	GetWorld()->GetTimerManager().SetTimer(creditNameTimers, this, &AGBCharacter::EndingCreditName, 1.0f, false, 3.0f);
}

void AGBCharacter::EndingCreditName()
{
	if (GBPlayerController != nullptr)
	{
		auto nextWidget = Cast<UNextLevelWidget>(GBPlayerController->GetNextLevelWidget());
		if (nextWidget != nullptr)
		{
			UWidgetAnimation* nameAnim = nextWidget->GetShowNameAnimation();
			if (nameAnim != nullptr)
			{
				nextWidget->PlayAnimation(nameAnim);
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(creditNameTimers);
	GetWorld()->GetTimerManager().SetTimer(creditUETimers, this, &AGBCharacter::EndingCreditUE, 1.0f, false, 4.5f);
}


void AGBCharacter::EndingCreditUE()
{
	if (GBPlayerController != nullptr)
	{
		auto nextWidget = Cast<UNextLevelWidget>(GBPlayerController->GetNextLevelWidget());
		if (nextWidget != nullptr)
		{
			UWidgetAnimation* ueAnim = nextWidget->GetShowUEAnimation();
			if (ueAnim != nullptr)
			{
				nextWidget->PlayAnimation(ueAnim);
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(creditUETimers);
	GetWorld()->GetTimerManager().SetTimer(returnTitle, this, &AGBCharacter::ReturnTitle, 1.0f, false, 7.0f);
}


void AGBCharacter::Grappling()		// ������ �� �� �ִ� �ڽ� �ݸ����� �ִ��� ã��
{
	FHitResult hitResult;									// ������ �浹�� Ž���� ���, ���õ� ������ ���� ����
	FVector start = aimCamComp->GetComponentLocation();
	FVector end = start + aimCamComp->GetForwardVector() * 1500;
	FCollisionQueryParams params(NAME_None, false, this);	// Ž�� ����� ���� ���� ���� ��Ƶ� ����ü

	bool bResult = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params);

	if (bResult)
	{
		//	�ڽ� �ݸ����� �ε����� Ż �� ����
		if (hitResult.GetComponent()->IsA(UBoxComponent::StaticClass()))
		{
			GBLOG(Error, TEXT("Hit the Collision !"));
			UGameplayStatics::PlaySound2D(GetWorld(), grapSound, 0.9f);
			grabPoint = hitResult.ImpactPoint;
			GBAnim->isGrappling = true;
			cable->SetVisibility(true);
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
	}

	// ���̾ ���� �������� Line Trace ����� ��ο�
	//DrawDebugLine(GetWorld(), start, end, FColor::Orange, false, 2.0f);
}


void AGBCharacter::GrapplingEnd()	
{
	if (GBAnim->isGrappling)
	{
		isJumping = true;	// �����Ҷ� ȸ���ϵ��� 

		GBAnim->isGrappling = false;
		cable->SetVisibility(false);
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

FVector AGBCharacter::GetUnitDirection(const FVector& Start, const FVector& End)	// ����Ÿ�� ���� ���� ��ġ ����ȭ�����ִ� �Լ�
{
	// Calculate the direction vector from StartPoint to EndPoint
	FVector Direction = (End - Start);

	// Check if the direction vector has a non-zero length to avoid division by zero
	if (Direction.SizeSquared() > SMALL_NUMBER)
	{
		// Normalize the direction vector to make it a unit vector
		Direction.Normalize();
	}
	else
	{
		// Handle the case where StartPoint and EndPoint are the same or very close
		Direction = FVector::ZeroVector;
	}

	return Direction;
}

float AGBCharacter::GetHPRatio()
{
	GBLOG(Warning, TEXT("current HP : %f, MAX HP : %f = %f"), current_HP, max_HP, current_HP / max_HP);
	return current_HP / max_HP;
}

void AGBCharacter::ReturnTitle()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title_Level"));
	GetWorld()->GetTimerManager().ClearTimer(returnTitle);
}


// �̵� �� ī�޶� ���� �Լ��� ============================================
void AGBCharacter::UpDown(float value)
{
	AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::X), value);
}

void AGBCharacter::LeftRight(float value)
{
	AddMovementInput(FRotationMatrix(FRotator(0, GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::Y), value);
}

void AGBCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AGBCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AGBCharacter::Jump()	// Jump�Լ� �������̵�(���� ��Ÿ�ָ� ���߰�, �޺� ���� �ʱ�ȭ �ڵ� �߰�)
{
	Super::Jump();

	isJumping = true;		// �����Ҷ� ȸ���ϵ����ϴ� ����
	StopAnimMontage(currentMontage);
	OnResetAttack();
}

void AGBCharacter::Run()
{
	auto moveMent = GetCharacterMovement();
	moveMent->MaxWalkSpeed = runSpeed;		// �޸��� ���·� ����
}

void AGBCharacter::Walk()
{
	auto moveMent = GetCharacterMovement();
	moveMent->MaxWalkSpeed = walkSpeed;		// �ȱ� ���·� ����
}

// ���� �ִϸ��̼� ���
void AGBCharacter::Shield()
{
	isGuarding = true;
	currentMontage = ShieldMontage;
	PlayAnimMontage(currentMontage);
}

// ���콺 ��Ŭ�� ���� ���� ����
void AGBCharacter::ShieldEnd()
{
	isGuarding = false;
	StopAnimMontage(currentMontage);
}


// ī�޶� �� ĳ���� ȸ�� ������ ����
void AGBCharacter::CameraSetting()
{
	springArm->SetRelativeRotation(FRotator(-40.0f, 0, 0));
	springArm->TargetArmLength = 450.0f;

	// ���콺�� �����̸� ĳ���ʹ� ȸ������ �ʰ�, ������ ���ֱ�
	// ī�޶� �����밡 ���콺 �����ӿ� ���� ȸ��
	springArm->bUsePawnControlRotation = true;		// �� ���� ȸ�� ���(�����ϸ�, ���� ��/��Ʈ�ѷ� ȸ�� ���� ����� ������)
	springArm->bInheritPitch = true;				// ī�޶� ����(��ġ ��� = Y�� ȸ�� ���)
	springArm->bInheritRoll = true;					// ī�޶� ����(�� ��� =  X�� ȸ�� ���)
	springArm->bInheritYaw = true;					// ī�޶� ����(�� ��� = Z�� ȸ�� ���)
	springArm->bDoCollisionTest = true;				// �ݸ��� �׽�Ʈ �����Ͽ�, ī�޶� Ŭ������ �Ͼ�� ���� ����
	bUseControllerRotationYaw = false;				// ��Ʈ�ѷ� �� ȸ�� ����

	// ĳ���� �����Ʈ ������Ʈ�� OrientRotationMovement �������, ĳ���Ͱ� �����̴� �������� ĳ���� �ڵ� ȸ��
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// UseControllerDesiredRotation �Ӽ��� üũ�ϸ� ��Ʈ�� ȸ���� ����Ű�� �������� ĳ���Ͱ� �ε巴�� ȸ���Ѵ�.
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 720.0f, 0);	// ȸ�� �ӵ�

	aimSpringArm->SetRelativeLocation(FVector(0, 0, 70));
	aimSpringArm->TargetArmLength = 0.0f;

	aimSpringArm->bUsePawnControlRotation = true;		// �� ���� ȸ�� ���(�����ϸ�, ���� ��/��Ʈ�ѷ� ȸ�� ���� ����� ������)
	aimSpringArm->bInheritPitch = true;				// ī�޶� ����(��ġ ��� = Y�� ȸ�� ���)
	aimSpringArm->bInheritRoll = true;					// ī�޶� ����(�� ��� =  X�� ȸ�� ���)
	aimSpringArm->bInheritYaw = true;					// ī�޶� ����(�� ��� = Z�� ȸ�� ���)
	aimSpringArm->bDoCollisionTest = true;				// �ݸ��� �׽�Ʈ �����Ͽ�, ī�޶� Ŭ������ �Ͼ�� ���� ����
}

void AGBCharacter::animMontageSetting()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackMontage_A(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Attack_Melee_A_Montage.Attack_Melee_A_Montage'"));
	if (attackMontage_A.Succeeded())
	{
		AttackMontage_A = attackMontage_A.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackMontage_B(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Attack_Melee_B_Montage.Attack_Melee_B_Montage'"));
	if (attackMontage_B.Succeeded())
	{
		AttackMontage_B = attackMontage_B.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackMontage_C(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Attack_Melee_C_Montage.Attack_Melee_C_Montage'"));
	if (attackMontage_C.Succeeded())
	{
		AttackMontage_C = attackMontage_C.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackMontage_D(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Attack_GuardBreak_A_Montage.Attack_GuardBreak_A_Montage'"));
	if (attackMontage_D.Succeeded())
	{
		AttackMontage_D = attackMontage_D.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackAir(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Attack_Melee_Air_Montage.Attack_Melee_Air_Montage'"));
	if (attackAir.Succeeded())
	{
		AttackAirMontage = attackAir.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> shield(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Ability_Block_Montage.Ability_Block_Montage'"));
	if (shield.Succeeded())
	{
		ShieldMontage = shield.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> hit(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/HitReact_Montage.HitReact_Montage'"));
	if (hit.Succeeded())
	{
		hitMontage = hit.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> shock(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/shock_Montage.shock_Montage'"));
	if (shock.Succeeded())
	{
		shockMontage = shock.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> death(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Player_Death.Player_Death'"));
	if (death.Succeeded())
	{
		deathMontage = death.Object;
	}
}
