// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "ABCharacterWidget.h"
#include "Components/WidgetComponent.h"
#include "ABAIController.h"
#include "DrawDebugHelpers.h"
#include "ABCharacterSetting.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "VisualLogger/VisualLogger.h"
#include "ABGameInstance.h"


// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Character Stat"));
	hpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP Bar"));

	springArm->SetupAttachment(GetCapsuleComponent());
	camComp->SetupAttachment(springArm);
	hpBarWidget->SetupAttachment(GetMesh());

	springArm->TargetArmLength = 400.0f;
	springArm->SetRelativeRotation(FRotator(-15, 0, 0));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88.0f), FRotator(0, -90, 0));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (tempMesh.Succeeded()) {

		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> warrior_anim(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Warrior.ABP_Warrior_C'"));
	if (warrior_anim.Succeeded()) {

		GetMesh()->SetAnimInstanceClass(warrior_anim.Class);
	}

	SetControlMode(EControlMode::DIABLO);

	armLengthSpeed = 3.0f;
	armRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	IsAttacking = false;	// ���� ������ ?
	MaxCombo = 4;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 80.0f;		// �Ǽ��� ��, ���� ���� 80���� ����
	AttackRadius = 50.0f;

	hpBarWidget->SetRelativeLocation(FVector(0, 0, 180.0f));	// HP_Bar�� ���� ��ġ ����
	hpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);			// HP_Bar�� �׻� �÷��̾ ���� ������ Screen���� ����

	static ConstructorHelpers::FClassFinder<UUserWidget>HP(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HPBar.UI_HPBar_C'"));
	if (HP.Succeeded())
	{
		hpBarWidget->SetWidgetClass(HP.Class);
		hpBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));	// �۾��� ������ ũ�Ⱑ 150 * 50�̾����Ƿ� �Ȱ��� ����
	}

	// AI ��Ʈ�ѷ� Ŭ����
	// ���� AI�� ���� ��Ʈ�ѵ� �� �⺻���� ����� AI ��Ʈ�ѷ� Ŭ����
	AIControllerClass = AABAIController::StaticClass();

	// AI �ڵ�����
	// ���� ���� �� AI ��Ʈ�ѷ��� ���� ���� �ñ�(���� ����, ���� �� ��...)�� ����
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ���⸦ ó������ ���� ���� �ʰ�, ���� �ֿ���ֵ��� �ڵ� ����(BeginPlay()�� �ۼ�)

	// �÷��̾�� NPC ���� �� ����
	AssetIndex = 4;	// �÷��̾� ĳ���ʹ� INDEX = 4�� ĳ���� �ּ� ���
	SetActorHiddenInGame(true);				// ĳ���͸� ó�� ������ ��, �Ⱥ��̵��� ��.
	hpBarWidget->SetHiddenInGame(true);		// ĳ������ ü�¹� UI�� ó�� ������ ��, �Ⱥ��̵��� ��.

	SetCanBeDamaged(false);					// ������ bCanBeDamaged = false ������, �𸮾� ���� 5������ Private�� ����Ǿ� 
											// SetCanBeDamaged(false)�� ����, GetCanBeDamaged()�� ������ ���� ����

	DeadTimer = 5.0f;		// DEAD ������Ʈ�� �� ��, �Լ��� ȣ���� �ð�
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayer = IsPlayerControlled();	// �÷��̾� ��Ʈ�ѷ����� �˷���
	
	if (bIsPlayer)
	{
		// �÷��̾� ��Ʈ�ѷ� ��������
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(ABPlayerController != nullptr);
	}
	else {
		// AI ��Ʈ�ѷ� ��������
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(ABAIController != nullptr);
	}

	auto DefaultSetting = GetDefault<UABCharacterSetting>();	// ArenaBattleSetting ��⿡ �ִ� ĳ���� �ּ� �ּ�

	if (bIsPlayer)
	{
		auto playerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(playerState != nullptr);
		AssetIndex = playerState->GetCharacterIndex();		// �÷��̾� ĳ���ʹ� Select �������� ������ �ε����� ���̷�Ż �޽÷� ����
	}

	else 
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);		// NPC ĳ���ʹ� �ε��� �������� �ּ� ĳ���� �����

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];		// �ּ� ���
	auto GI = Cast<UABGameInstance>(GetGameInstance());						// ���� �ν��Ͻ� ��������
	
	ABCHECK(GI != nullptr);
	// StreamableManager.RequestAsyncLoad �񵿱�� �ּ� �ε�
	// FStreamableDelegate::CreateUObject�� �Ｎ ��������Ʈ �����Ͽ� �Լ� ȣ��
	AssetStreamingHandle = GI->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);

	//FName WeaponSocket(TEXT("hand_rSocket"));
	//
	//auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	//if (CurWeapon != nullptr)
	//{
	//	CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	//}
}


// ĳ���� ������Ʈ ���� �Լ�
void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;	// ���� ĳ���� ������Ʈ ����

	switch (CurrentState)
	{
		case ECharacterState::LOADING:				// ĳ���� ���� �� ������Ʈ
		{
			if (bIsPlayer)	// �÷��̾��̸� ?
			{
				DisableInput(ABPlayerController);	// �Է� �Ұ�

				ABPlayerController->GetHudWidget()->BindCharacterStat(stat);	// ĳ���� ���� ������Ʈ�� HUD ���� ����

				auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
				ABCHECK(ABPlayerState != nullptr);
				stat->SetNewLevel(ABPlayerState->GetCharacterLevel());	// �ش� ������ ������ ���̺��� �� �����ͷ� ĳ���� ������ ����
			}

			else       // NPC�̸�
			{
				auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());				// ���� ��� ��������
				ABCHECK(ABGameMode != nullptr);
				int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));	// �Ҽ��� ù�ڸ� �ø��Ͽ� ���� ��ȯ

				int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);	// TargetLevel�� 1 ~ 20 ������ ������ ��������
				ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
				stat->SetNewLevel(FinalLevel);
			}

			SetActorHiddenInGame(true);				// ĳ���� �Ⱥ��̵���
			hpBarWidget->SetHiddenInGame(true);		// ü�¹� UI �Ⱥ��̵���
			SetCanBeDamaged(false);					// ������ ���� �ʵ���
			break;
		}

		case ECharacterState::READY:
		{
			SetActorHiddenInGame(false);			// ĳ���Ͱ� ���̵���
			hpBarWidget->SetHiddenInGame(false);	// ü�¹� UI�� ���̵���
			SetCanBeDamaged(true);					// ����� ���� �� ����

			stat->OnHPIsZero.AddLambda([this]() -> void {		// ü���� 0�̵Ǹ� DEAD ������Ʈ�� ����
					SetCharacterState(ECharacterState::DEAD);
			});

			auto CharacterWidget = Cast<UABCharacterWidget>(hpBarWidget->GetUserWidgetObject());
			ABCHECK(CharacterWidget != nullptr);

			// ü�¹� UI�� ĳ������ stat�� ���ε�, ĳ������ ���� ü���� ����� stat�� ���� UI�� ���α׷����ٿ� ����
			CharacterWidget->BindCharacterStat(stat);	

			if (bIsPlayer)	// �÷��̾��̸�
			{
				SetControlMode(EControlMode::DIABLO);
				GetCharacterMovement()->MaxWalkSpeed = 600.0f;
				EnableInput(ABPlayerController);

				// ĳ���Ͱ� �������ϸ� ü��ȸ��
				// ABPlayerController���� �ۼ��� ��������Ʈ(PlayerState�� AddExp���� ������ �ߴ��� True ���޹����� �����.)
				ABPlayerController->OnLevelUP.AddLambda([this]() -> void {

					auto playerState = Cast<AABPlayerState>(GetPlayerState());
					ABCHECK(playerState != nullptr);

					stat->SetNewLevel(playerState->GetCharacterLevel());	// �ش� ������ ������ ���̺��� �� �����ͷ� ĳ���� ������ ����
				});
			}

			else			// NPC�̸�
			{
				SetControlMode(EControlMode::NPC);
				GetCharacterMovement()->MaxWalkSpeed = 400.0f;
				ABAIController->RunAI();
			}

			break;
		}

		case ECharacterState::DEAD:
		{
			SetActorEnableCollision(false);			// �浹 �ݸ��� ����
			GetMesh()->SetHiddenInGame(false);		// ĳ���Ͱ� ���̵���
			hpBarWidget->SetHiddenInGame(true);		// ü�¹� UI�� �Ⱥ��̵���
			ABAnim->SetDeadAnim();					// ��� �ִϸ��̼� ����
			SetCanBeDamaged(false);					// ����� ���� �ʵ���

			if (bIsPlayer)
			{
				DisableInput(ABPlayerController);		// �Է� �Ұ�����
			}
			else
			{
				ABAIController->StopAI();	// �����̺�� Ʈ�� ���߱�
			}

			// DeadTimer(= 5��) ��, �÷��̾�� ���� �����, NPC�� ���� ����
			GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
				if (bIsPlayer)
				{
					ABPlayerController->ShowResultUI();		// ��� UI ����
				}
				else
				{
					Destroy();
				}
			}), DeadTimer, false);

			break;
		}
	}
}

// NPC�� �׾��� �� �ִ� ����ġ
int32 AABCharacter::GetExp() const
{
	return stat->GetDropExp();
}

ECharacterState AABCharacter::GetCharacterState() const
{
	return CurrentState;
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	springArm->TargetArmLength = FMath::FInterpTo(springArm->TargetArmLength, armLengthTo, DeltaTime, armLengthSpeed);

	switch (currentMode)
	{
		case EControlMode::DIABLO :
			springArm->SetRelativeRotation(FMath::RInterpTo(springArm->GetRelativeRotation(), armRotationTo, DeltaTime, armRotationSpeed));

			if (DirectionToMove.SizeSquared() > 0.0f)
			{
				GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
				AddMovementInput(DirectionToMove);
			}
			break;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(ABAnim != nullptr);

	// ��Ÿ�ְ� ������ �ڵ����� ����
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// �� OnMontageEnded ��������Ʈ�� ����� ��, OnAttackMontageEnded �Լ��� ����ǰ� �Ѱ�ó��
	// OnNextAttackCheck ��������Ʈ�� ����� ��, ȣ��� �Լ��� ���ٽ����� �ۼ�
	// OnNextAttackCheckDelegate Broadcast �� �߻��Ҷ�
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
	
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)	// ���� ��ư�� �����ٸ�
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);	// ���� ��Ÿ�� ����(CurrentCombo)�� �ѱ��.
		}
	});

	// �浹 ���� Ž�� ��������Ʈ
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	// ü���� 0�� �Ǿ��� �� ȣ��Ǵ� ��������Ʈ�� ���ٽ����� �Լ� ����
	stat->OnHPIsZero.AddLambda([this]()-> void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();				// isDead = true�� �� -> ��� �ִϸ��̼� ����
		SetActorEnableCollision(false);		// �浹 ���� ����
		});
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AABCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AABCharacter::Attack);

	// ����Ī ��� ���� �Լ�(Shift + V)
	PlayerInputComponent->BindAction(TEXT("ViewChange"), IE_Pressed, this, &AABCharacter::ViewChange);
}


// 4���� �� �Է� ���� �Լ�
void AABCharacter::UpDown(float NewAxisValue) 
{
	switch (currentMode)
	{
		case EControlMode::GTA :
			AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::X), NewAxisValue);
			break;

		case EControlMode::DIABLO :
			DirectionToMove.X = NewAxisValue;
			break;
	}

	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	switch (currentMode)
	{
		case EControlMode::GTA :
			AddMovementInput(FRotationMatrix(FRotator(0, GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::Y), NewAxisValue);
			break;

		case EControlMode::DIABLO :
			DirectionToMove.Y = NewAxisValue;
			break;
	}

	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
}

void AABCharacter::Turn(float NewAxisValue)
{
	switch (currentMode)
	{
		case EControlMode::GTA:
			AddControllerYawInput(NewAxisValue);
			break;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	switch (currentMode)
	{
		case EControlMode::GTA:
			AddControllerPitchInput(NewAxisValue);
			break;
	}
}

// ���� �Է� �Լ�
void AABCharacter::Attack()
{
	if (IsAttacking) // ���� ��, ���� ��ư�� �� ������ ���
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));

		if (CanNextCombo) {
			IsComboInputOn = true;	// NextAttackNotify���� ���� ���� ���� true�� ����(= OnNextAttackCheck ��������Ʈ�� True�� Ȯ�εǾ� ���� ���� �ϰ���)
		}
	}

	else   // ù ������ ��,
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();	// CurrentCombo + 1 
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);	// ���� ����
		IsAttacking = true;
	}
}

// ���� ��Ÿ�� �����ٴ� �� ��Ÿ���� �Լ�
void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

// ���� ������ ����
void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;																// CanNextCombo = ���� �޺��� �̵� ���� ����
	IsComboInputOn = false;																// IsComboInputOn = �޺� �Է� ����

	// CurrentCombo���� ���� 0 ~ 3 ���ο� �ִ��� �˻��Ѵ�.
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));			// MaxCombo = �޺� ī������ �ִ�ġ

	// CurrentCombo + 1�� ���� ������ 1 ~ MaxCombo(4)�� �����Ѵ�.
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);					// CurrentCombo = ���� ���� ���� �޺� ī����
}

// ���� �ִϸ��̼� ����
void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

// �������� ��, ������ �浹�� �����Ǹ�
void AABCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG

	// ������ 50, Ž�� ���� ��ġ���� Ž�� �� ��ġ�� ���ϴ� ���͸� ���� ��, ������ ������ġ�� ���� ������ ������ �����ϸ� ���ϴ� ũ���� ĸ�� ����� ���� �� ����
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;			// ������ 200cm�Ÿ� Ž��
	FVector Center = GetActorLocation() + TraceVec * 0.5f;					// ������ ���� ��ġ
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;				// ���� ������ ����
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();		// Z�� ȸ�� ��ǥ�� ����
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;				// ���� ���� �� ���� Ž���Ǹ� ���, �ƴϸ� ����������
	float DebugLifeTime = 5.0f;												// ĸ�� 5�ʰ� ����

	DrawDebugCapsule(GetWorld(),	// ĸ�� ����� ����
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);


	// ���־� �ΰ�
	// ĳ������ ��ġ��  50cm�� �Ķ��� ��ü�� ǥ��
	UE_VLOG_LOCATION(this, ArenaBattle, Verbose, GetActorLocation(), 50.0f, FColor::Blue, TEXT("Attack Position"));

	// ���� ���� ������ ���� ��ü ǥ��
	UE_VLOG_CAPSULE(this, ArenaBattle, Verbose, GetActorLocation() - GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attack Area"));
#endif

	if (bResult)
	{
		if (HitResult.GetActor())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}

	if (bIsPlayer)
		ABLOG(Warning, TEXT("current HP : %f"), stat->GetHPRatio());
}

// ���Ϳ� ������� �����ϴ� TakeDamage �Լ� �������̵�
float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// AttackCheck() �Լ����� HitResult.GetActor()�� TakeDamage() �Լ��� ȣ��� ������ GetName() ���� ������ ���� �̸��� ��µ�.
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	stat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())	// �����ڰ� �÷��̾��ΰ� ?
		{
			auto controller = Cast<AABPlayerController>(EventInstigator);	// �������� ��Ʈ�ѷ� ��������
			ABCHECK(controller != nullptr, 0.0f);

			controller->NPCKill(this);	// �÷��̾ NPCKill �ߴٴ� �Լ� ȣ�� = NPCKill �Լ��� �÷��̾��� ����ġ�� �߰����ִ� �Լ�
		}
	}

	return FinalDamage;
}

// ���⸦ ���� �� �ִ��� Ȯ���ϴ� �Լ�(�׻� ���⸦ ���� �� �ֵ��� ����)
bool AABCharacter::CanSetWeapon()
{
	return true;
}

// ���⸦ �����ϴ� �Լ�
void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(NewWeapon != nullptr);

	FName WeaponSocket(TEXT("hand_rSocket"));

	if (CurrentWeapon != nullptr)		// ���� ���Ⱑ �ִٸ�
	{
		// ���� ����� ������Ʈ���� �� ���͸� �и��س���.
		// KeepWorldTransform�� ���� �������� ��ȯ�� ��ȯ�� ���� ���� ��Ҹ� �и��س��� ���� �ǹ��ϴ� �� ����.
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();	// ���� ����
		CurrentWeapon = nullptr;	// ���� ���� ����������� �����
	}

	if (NewWeapon != nullptr)	
	{
		// SnapToTargetNotIncludingScale : �־��� ����� �Ǵ� ���� Ʈ�������� ������ �Ű������� �����Ѵ�.
		// �̷��� �ϸ� ���Ⱑ �ƿ� ���̳ʿ��� ���ͷ� ���� �и��Ǿ� �ִ�.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}


// �÷��̾��� ���� ���� ��ȯ 
float AABCharacter::GetFinalAttackRange() const
{
	// ���Ⱑ ������ ������ ���� ������ ��ȯ, ���Ⱑ ������ �Ǽ� ������ ���� ���� ��ȯ
	return (CurrentWeapon != nullptr) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

// ���� ����� ��ȯ
float AABCharacter::GetFinalAttackDamage() const
{
	// ���Ⱑ ������ ĳ���� �⺻ ���ݷ� + ���� ���ݷ� vs ���Ⱑ ������ ĳ���� �⺻ ���ݷ� 
	float AttackDamage = (CurrentWeapon != nullptr) ? (stat->GetAttack() + CurrentWeapon->GetAttackDamage()) : stat->GetAttack();

	// ���Ⱑ ������ ������ ȿ��ġ(��質 ���ϰ�) vs ���Ⱑ ������ �⺻ ȿ��ġ 1 ���
	float AttackModifier = (CurrentWeapon != nullptr) ? CurrentWeapon->GetAttackModifier() : 1.0f;

	return AttackDamage * AttackModifier;
}

// �񵿱�� �ּ��� �ε��ϴ� ���� ������ ȣ��Ǵ� �Լ�
void AABCharacter::OnAssetLoadCompleted()
{
	// ���̷�Ż �޽� ��ο� �ִ� ���̷�Ż �޽ø� ������
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	
	AssetStreamingHandle.Reset();
	ABCHECK(AssetLoaded != nullptr);
	
	GetMesh()->SetSkeletalMesh(AssetLoaded);	// ������ ���̷�Ż �޽÷� ����

	SetCharacterState(ECharacterState::READY);	//	ĳ���� �ּ� �ε��� �������Ƿ�, READY ���·� ��ȯ
}


// ��� ���� �Լ�
void AABCharacter::ViewChange()
{
	switch (currentMode)
	{
		case EControlMode::GTA:
			GetController()->SetControlRotation(GetActorRotation());
			SetControlMode(EControlMode::DIABLO);
			break;

		case EControlMode::DIABLO:
			GetController()->SetControlRotation(springArm->GetRelativeRotation());
			SetControlMode(EControlMode::GTA);
			break;
	}
}

// ����Ī ��� ���� �Լ�
void AABCharacter::SetControlMode(EControlMode ControlMode)
{
	currentMode = ControlMode;

	switch (currentMode)
	{

		case EControlMode::GTA :
			// ���콺�� �����̸� ĳ���ʹ� ȸ������ �ʰ�, ������ ������
			// ī�޶� �����밡 ���콺 �����ӿ� ���� ȸ��
			//springArm->TargetArmLength = 450.0f;
			//springArm->SetRelativeRotation(FRotator::ZeroRotator);	// = FRotator(0, 0, 0);
			armLengthTo = 450.0f;
			springArm->bUsePawnControlRotation = true;				// �� ���� ȸ�� ���(�����ϸ�, ���� ��/��Ʈ�ѷ� ȸ�� ���� ����� ������)
			springArm->bInheritPitch = true;						// ī�޶� ����(��ġ ���)
			springArm->bInheritRoll = true;							// ī�޶� ����(�� ���)	
			springArm->bInheritYaw = true;							// ī�޶� ����(�� ���)
			springArm->bDoCollisionTest = true;						// �ݸ��� �׽�Ʈ �����Ͽ�, ī�޶� Ŭ������ �Ͼ�� �� ����
			bUseControllerRotationYaw = false;						// ��Ʈ�ѷ� �� ȸ�� ����

			// ĳ���� �����Ʈ ������Ʈ�� OrientRotationMovement �������, ĳ���Ͱ� �����̴� �������� ĳ���� �ڵ� ȸ��
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0, 720.0f, 0);	// ȸ�� �ӵ�
			break;

		case EControlMode::DIABLO :
			// ���� Ű�� �¿� Ű�� �����Ͽ� ĳ���� ȸ�� �� �̵�
			//springArm->TargetArmLength = 800.0f;
			//springArm->SetRelativeRotation(FRotator(-45.0f, 0, 0));
			armLengthTo = 800.0f;
			armRotationTo = FRotator(-45.0f, 0, 0);
			springArm->bUsePawnControlRotation = false;
			springArm->bInheritPitch = false;
			springArm->bInheritRoll = false;
			springArm->bInheritYaw = false;
			springArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false;

			// OrientRotationMovement �Ӽ��� ĳ���Ͱ� �����̴� �������� ĳ���͸� �ڵ� ȸ����Ű����, UseControllerDesiredRotation�� �ڵ� ȸ�� �����ش�.
			// UseControllerDesiredRotation �Ӽ��� üũ�ϸ� ��Ʈ�� ȸ���� ����Ű�� �������� ĳ���Ͱ� �ε巴�� ȸ���Ѵ�.
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0, 720.0f, 0);
			break;

		case EControlMode::NPC :
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0, 480.0f, 0);
			break;
	}
}
