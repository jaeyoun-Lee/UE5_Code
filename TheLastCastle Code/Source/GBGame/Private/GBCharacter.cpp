// Fill out your copyright notice in the Description page of Project Settings.


#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "GBAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "NiagaraComponent.h"
#include "GBHUDWidget.h"
#include "NextLevelWidget.h"
#include "CableComponent.h"			// 케이블 컴포넌트를 위한 헤더 파일
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
	GetCapsuleComponent()->SetCapsuleHalfHeight(93.762238f);	// 캡슐 컴포넌트 길이 조절
	GetCapsuleComponent()->SetCapsuleRadius(33.495377f);		// 캡슐 컴포넌트 반경 조절
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));		// Player 콜리전 프리셋 사용

	CameraSetting();	// 카메라 및 캐릭터 회전 움직임 세팅 함수
	animMontageSetting();

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;	// 캐릭터 속도 조절
	GetCharacterMovement()->JumpZVelocity = 650.0f;		// 캐릭터 점프 높이 조절
	
	canAttacking = true;	// 플레이어가 Enemy한테 맞아도 되는 상태인지 확인해주는 변수(감전 공격을 받는 중, 다른 공격을 받지 못하게하기 위함)
	IsAttacking = false;	// 공격 중인지 확인해주는 변수
	isJumping = false;		// Space바를 눌러 점프한 것인지 
	isGuarding = false;		// 가드 중인지 확인해주는 변수
	isDeath = false;
	IsShocking = false;		// 감전 공격을 맞았는지 확인해주는 변수
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
	cable->CableWidth = 3.0f;		// 케이블 컴포넌트의 두께 3cm로 설정

	shockLightning->SetVisibility(false);
}


void AGBCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GBAnim = Cast<UGBAnimInstance>(GetMesh()->GetAnimInstance());
	GBCHECK(GBAnim != nullptr);

	// bNextAttack 노티파이를 지나면 UGBAnimInstance::AnimNotify_bNextAttack()가 호출되고,
	// UGBAnimInstance::AnimNotify_bNextAttack() 함수에서 OnNextAttackCheck 델리게이트 호출
	GBAnim->OnNextAttackCheck.AddLambda([this]() -> void {
			OnNextAttack();
		});

	// ResetCombo 노티파이를 지나면 UGBAnimInstance::AnimNotify_ResetCombo가 호출되고,
	// UGBAnimInstance::AnimNotify_ResetCombo 함수에서 OnResetCheck 델리게이트 호출
	// 콤보 공격이 입력되지 않았거나, 최대 콤보가 끝났으면 호출되는 델리게이트
	GBAnim->OnResetCheck.AddLambda([this]() -> void {
			GBLOG(Warning, TEXT("Called by OnResetCheck"));
			OnResetAttack();
		});

	// 공격 애님 몽타주에서 공격을 맞았는지 확인할때 호출되는 노티파이
	GBAnim->OnAttackHitCheck.AddUObject(this, &AGBCharacter::AttackCheck);		

	// 전기 공격을 맞고, 스턴이 끝날때 호출되는 함수
	GBAnim->OnShockCheck.AddUObject(this, &AGBCharacter::OnEndShock);

	// 플레이어 사망 시, 호출되는 람다
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

		// SetActorEnableCollision(false);			// 충돌 설정 제거
		PlayAnimMontage(deathMontage, 0.7f);	// 사망 애니메이션 재생
		DisableInput(GBPlayerController);		// 입력불가
		isDeath = true;
		});

	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));	// 게임 스테이트 가져오기
	if (gameState)
	{
		GBPlayerController->GetHUDWidget()->UpdateEnemyCnt(gameState->GetEnemyCount());
	}

	if (gameState != nullptr)
	{
		// 게임 보스몹 처리 후, 3초 뒤에 GameClear() 함수 호출
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

		GetCharacterMovement()->AddForce(total);		// 갈고리를 건 방향으로 날아감
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

	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AGBCharacter::Run);	// 누르고 있으면 달리기 모드 On
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AGBCharacter::Walk);	// 떼면 달리기 모드 Off

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AGBCharacter::Attack);	
	PlayerInputComponent->BindAction(TEXT("Shield"), IE_Pressed, this, &AGBCharacter::Shield);
	PlayerInputComponent->BindAction(TEXT("Shield"), IE_Released, this, &AGBCharacter::ShieldEnd);

	PlayerInputComponent->BindAction(TEXT("Grappling"), IE_Pressed, this, &AGBCharacter::Grappling);
	PlayerInputComponent->BindAction(TEXT("Grappling"), IE_Released, this, &AGBCharacter::GrapplingEnd);
}

void AGBCharacter::Attack()
{
	if (!GBAnim->isAir && !GBAnim->isGrappling)		// 땅에 있으면서,
	{
		if (IsAttacking)		// 공격 중이면
		{
			isComboInputOn = true;
		}

		else                   // 첫 공격이면
		{
			IsAttacking = true;
			currentCombo = 1;
			PlayAnimMontage(AttackMontage_A);
		}
	}

	else     // 공중에 있으면서
	{
		if (!IsAttacking)	// 첫 공격이면(공중에서 공격 1번만 할 수 있도록)
		{
			GrapplingEnd();			// 와이어 타고 날라가면서 공격 하면, 와이어 끊어지도록
			attackRadius = 85.0f;
			IsAttacking = true;
			PlayAnimMontage(AttackAirMontage);
			GBAnim->bAirAttack = true;
		}
	}
}


void AGBCharacter::AttackCheck()
{
	FHitResult hitResult;									// 물리적 충돌이 탐지된 경우, 관련된 정보를 담을 구조체
	FCollisionQueryParams params(NAME_None, false, this);	// 탐색 방법에 대한 설정 값을 모아둔 구조체

	bool bResult = GetWorld()->SweepSingleByChannel(	// Trace 채널로 물리적 충돌 여부 확인
		hitResult,													
		GetActorLocation(),											// 현재 캐릭터 위치에서 시작하여
		GetActorLocation() + GetActorForwardVector() * attackRange,	// 현재 캐릭터 전방 200cm 탐색
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,					// Attack 트레이스 사용
		FCollisionShape::MakeSphere(attackRadius),					// 반지름 50cm의 구체로 탐지한다.
		params);

#if ENABLE_DRAW_DEBUG	// 디버그 드로잉이 가능하게 하기위한 코드
	
	FVector traceVec = GetActorForwardVector() * attackRange;		// 캐릭터의 시선방향 벡터
	FVector center = GetActorLocation() + traceVec * 0.5f;			// 캐릭터 현재 위치에서 벡터 중점 위치
	float halfHeight = attackRange * 0.5f + attackRadius;			// 캡슐 절반 높이
	FQuat capsuleRot = FRotationMatrix::MakeFromZ(traceVec).ToQuat();	// 캡슐을 그리기위해 필요한 회전 좌표축 
	FColor drawColor = bResult ? FColor::Green : FColor::Red;			// 공격 성공 시, 초록을, 공격 실패 시, 빨강으로 디버깅 드로우
	float debugLifeTime = 5.0f;											// 디버깅 드로우(캡슐 모양) 유지 시간

	DrawDebugCapsule(GetWorld(),
		center,
		halfHeight,
		attackRadius,
		capsuleRot,
		drawColor,
		false,
		debugLifeTime);

#endif		// ~#if ENABLE_DRAW_DEBUG  디버그 드로잉이 가능하게 하기위한 코드

	if (bResult)	// 충돌 감지되면(공격에 맞으면)
	{
		if (hitResult.GetActor())	// 탐지된 액터를 가져옴
		{
			UGameplayStatics::PlaySound2D(GetWorld(), beAttackSFX);

			GBLOG(Warning, TEXT("Hit Actor Name : %s"), *hitResult.GetActor()->GetName());
			float randDamage = 35 * (FMath::RandRange(0.5, 1.2));		// 공격 데미지 랜덤으로 설정

			FDamageEvent DamageEvent;		// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
			hitResult.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()는 공격 당한 액터를 의미한다.
		}
	}
}

// bNextAttack 노티파이를 지나면 델리게이트에서 호출하는 함수
void AGBCharacter::OnNextAttack()
{
	GBLOG(Warning, TEXT("Called by OnNextAttackCheck"));
	GBCHECK(!GBAnim->isAir);
	// 콤보 공격이 입력되었으면
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

// ResetCombo 노티파이를 지나면 델리게이트에서 호출하는 함수
void AGBCharacter::OnResetAttack()
{
	currentCombo = 0;
	isComboInputOn = false;
	IsAttacking = false;
	attackRadius = 70.0f;
}


// 공격 당하여 받은 데미지 처리하는 함수
float AGBCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (canAttacking)
	{
		GBLOG(Error, TEXT("Character TakeDamage"));
		float finalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		if (isGuarding)
		{
			finalDamage *= 0.2;		// 가드 상태이면 대미지가 덜 들어오게함
		}

		GBLOG(Warning, TEXT("Actor : %s, took Damage : %f"), *GetName(), finalDamage);
		current_HP -= finalDamage;			// 현재 체력에서 입은 피해를 빼준다.
		isGuarding = false;					// 공격 당했으니, 가드 깨짐

		if (IsShocking)		// 전기 공격에 맞은경우,
		{
			canAttacking = false;
			PlayAnimMontage(shockMontage, 1.0f);
			shockLightning->SetVisibility(true);
			DisableInput(GBPlayerController);		// 스턴에 걸린 상태로 만들기 위해서 입력불가
		}

		else               // 다른 공격에 맞은경우,
		{
			PlayAnimMontage(hitMontage, 1.0f);
		}

		//  변경된 체력을 UI에 적용해주는 UpdatePlayerHP() 함수 호출
		GBPlayerController->GetHUDWidget()->UpdatePlayerHP(GetHPRatio());

		return finalDamage;
	}

	else
		return 0;
}

// 전기 공격을 맞고, 스턴이 끝날때 호출되는 함수
void AGBCharacter::OnEndShock()
{
	canAttacking = true;
	IsShocking = false;
	shockLightning->SetVisibility(false);
	StopAnimMontage(shockMontage);
	EnableInput(GBPlayerController);		// 스턴 상태 해제 = 입력가능
}

void AGBCharacter::GameClear()
{
	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));

	if (gameState != nullptr)
	{
		if (GBPlayerController != nullptr)
		{
			GBPlayerController->ShowSucceededUI();
			GetCharacterMovement()->MaxWalkSpeed = 0;		// 게임 Clear UI가 나왔을 때, 달리는 중이면 계속 달린다. 이를 막기 위함.
			GrapplingEnd();									// 게임 Clear UI가 나왔을 때 매달려있는거 해제하기 위해서

			GetWorld()->GetTimerManager().SetTimer(creditStartTimers, this, &AGBCharacter::EndingCredit, 1.0f, false, 2.5f);
		}
	}
}

void AGBCharacter::EndingCredit()
{
	if (GBPlayerController != nullptr)
	{
		GBPlayerController->ShowNextUI();				// 화면이 흐릿해지는 UI 생성
		GBPlayerController->bShowMouseCursor = false;	// 마우스 커서 X

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


void AGBCharacter::Grappling()		// 갈고리를 걸 수 있는 박스 콜리전이 있는지 찾음
{
	FHitResult hitResult;									// 물리적 충돌이 탐지된 경우, 관련된 정보를 담을 변수
	FVector start = aimCamComp->GetComponentLocation();
	FVector end = start + aimCamComp->GetForwardVector() * 1500;
	FCollisionQueryParams params(NAME_None, false, this);	// 탐색 방법에 대한 설정 값을 모아둔 구조체

	bool bResult = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params);

	if (bResult)
	{
		//	박스 콜리전에 부딪혀야 탈 수 있음
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

	// 와이어가 날라갈 방향으로 Line Trace 디버깅 드로우
	//DrawDebugLine(GetWorld(), start, end, FColor::Orange, false, 2.0f);
}


void AGBCharacter::GrapplingEnd()	
{
	if (GBAnim->isGrappling)
	{
		isJumping = true;	// 점프할때 회전하도록 

		GBAnim->isGrappling = false;
		cable->SetVisibility(false);
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

FVector AGBCharacter::GetUnitDirection(const FVector& Start, const FVector& End)	// 갈고리타고 날라갈 곳의 위치 정규화시켜주는 함수
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


// 이동 및 카메라 세팅 함수들 ============================================
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

void AGBCharacter::Jump()	// Jump함수 오버라이드(공격 몽타주를 멈추고, 콤보 공격 초기화 코드 추가)
{
	Super::Jump();

	isJumping = true;		// 점프할때 회전하도록하는 변수
	StopAnimMontage(currentMontage);
	OnResetAttack();
}

void AGBCharacter::Run()
{
	auto moveMent = GetCharacterMovement();
	moveMent->MaxWalkSpeed = runSpeed;		// 달리기 상태로 변경
}

void AGBCharacter::Walk()
{
	auto moveMent = GetCharacterMovement();
	moveMent->MaxWalkSpeed = walkSpeed;		// 걷기 상태로 변경
}

// 쉴드 애니메이션 재생
void AGBCharacter::Shield()
{
	isGuarding = true;
	currentMontage = ShieldMontage;
	PlayAnimMontage(currentMontage);
}

// 마우스 우클릭 떼면 쉴드 해제
void AGBCharacter::ShieldEnd()
{
	isGuarding = false;
	StopAnimMontage(currentMontage);
}


// 카메라 및 캐릭터 회전 움직임 세팅
void AGBCharacter::CameraSetting()
{
	springArm->SetRelativeRotation(FRotator(-40.0f, 0, 0));
	springArm->TargetArmLength = 450.0f;

	// 마우스를 움직이면 캐릭터는 회전하지 않고, 가만히 서있기
	// 카메라 지지대가 마우스 움직임에 따라 회전
	springArm->bUsePawnControlRotation = true;		// 폰 제어 회전 사용(가능하면, 폰의 뷰/컨트롤러 회전 값을 사용할 것인지)
	springArm->bInheritPitch = true;				// 카메라 세팅(피치 상속 = Y축 회전 상속)
	springArm->bInheritRoll = true;					// 카메라 세팅(롤 상속 =  X축 회전 상속)
	springArm->bInheritYaw = true;					// 카메라 세팅(요 상속 = Z축 회전 상속)
	springArm->bDoCollisionTest = true;				// 콜리전 테스트 수행하여, 카메라 클리핑이 일어나는 것을 방지
	bUseControllerRotationYaw = false;				// 컨트롤러 요 회전 끄기

	// 캐릭터 무브먼트 컴포넌트의 OrientRotationMovement 기능으로, 캐릭터가 움직이는 방향으로 캐릭터 자동 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// UseControllerDesiredRotation 속성을 체크하면 컨트롤 회전이 가리키는 방향으로 캐릭터가 부드럽게 회전한다.
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 720.0f, 0);	// 회전 속도

	aimSpringArm->SetRelativeLocation(FVector(0, 0, 70));
	aimSpringArm->TargetArmLength = 0.0f;

	aimSpringArm->bUsePawnControlRotation = true;		// 폰 제어 회전 사용(가능하면, 폰의 뷰/컨트롤러 회전 값을 사용할 것인지)
	aimSpringArm->bInheritPitch = true;				// 카메라 세팅(피치 상속 = Y축 회전 상속)
	aimSpringArm->bInheritRoll = true;					// 카메라 세팅(롤 상속 =  X축 회전 상속)
	aimSpringArm->bInheritYaw = true;					// 카메라 세팅(요 상속 = Z축 회전 상속)
	aimSpringArm->bDoCollisionTest = true;				// 콜리전 테스트 수행하여, 카메라 클리핑이 일어나는 것을 방지
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
