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

	IsAttacking = false;	// 공격 중인지 ?
	MaxCombo = 4;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 80.0f;		// 맨손일 때, 공격 범위 80으로 설정
	AttackRadius = 50.0f;

	hpBarWidget->SetRelativeLocation(FVector(0, 0, 180.0f));	// HP_Bar가 보일 위치 설정
	hpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);			// HP_Bar가 항상 플레이어를 향해 보도록 Screen모드로 설정

	static ConstructorHelpers::FClassFinder<UUserWidget>HP(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HPBar.UI_HPBar_C'"));
	if (HP.Succeeded())
	{
		hpBarWidget->SetWidgetClass(HP.Class);
		hpBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));	// 작업한 공간의 크기가 150 * 50이었으므로 똑같이 설정
	}

	// AI 컨트롤러 클래스
	// 폰이 AI에 의해 컨트롤될 때 기본으로 사용할 AI 컨트롤러 클래스
	AIControllerClass = AABAIController::StaticClass();

	// AI 자동빙의
	// 폰의 생성 및 AI 컨트롤러에 의한 빙의 시기(레빌 시작, 스폰 시 등...)를 결정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 무기를 처음부터 갖고 있지 않고, 직접 주울수있도록 코드 변경(BeginPlay()에 작성)

	// 플레이어와 NPC 구분 및 세팅
	AssetIndex = 4;	// 플레이어 캐릭터는 INDEX = 4의 캐릭터 애셋 사용
	SetActorHiddenInGame(true);				// 캐릭터를 처음 생성할 때, 안보이도록 함.
	hpBarWidget->SetHiddenInGame(true);		// 캐릭터의 체력바 UI도 처음 생성할 때, 안보이도록 함.

	SetCanBeDamaged(false);					// 기존에 bCanBeDamaged = false 였으나, 언리얼 엔진 5에서는 Private로 변경되어 
											// SetCanBeDamaged(false)로 변경, GetCanBeDamaged()로 가져올 수도 있음

	DeadTimer = 5.0f;		// DEAD 스테이트가 된 후, 함수를 호출할 시간
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayer = IsPlayerControlled();	// 플레이어 컨트롤러인지 알려줌
	
	if (bIsPlayer)
	{
		// 플레이어 컨트롤러 가져오기
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(ABPlayerController != nullptr);
	}
	else {
		// AI 컨트롤러 가져오기
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(ABAIController != nullptr);
	}

	auto DefaultSetting = GetDefault<UABCharacterSetting>();	// ArenaBattleSetting 모듈에 있는 캐릭터 애셋 주소

	if (bIsPlayer)
	{
		auto playerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(playerState != nullptr);
		AssetIndex = playerState->GetCharacterIndex();		// 플레이어 캐릭터는 Select 레벨에서 선택한 인덱스의 스켈레탈 메시로 생성
	}

	else 
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);		// NPC 캐릭터는 인덱스 랜덤으로 애셋 캐릭터 만들기

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];		// 애셋 경로
	auto GI = Cast<UABGameInstance>(GetGameInstance());						// 게임 인스턴스 가져오기
	
	ABCHECK(GI != nullptr);
	// StreamableManager.RequestAsyncLoad 비동기로 애셋 로딩
	// FStreamableDelegate::CreateUObject로 즉석 델리게이트 생성하여 함수 호출
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


// 캐릭터 스테이트 설정 함수
void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;	// 현재 캐릭터 스테이트 변경

	switch (CurrentState)
	{
		case ECharacterState::LOADING:				// 캐릭터 생성 전 스테이트
		{
			if (bIsPlayer)	// 플레이어이면 ?
			{
				DisableInput(ABPlayerController);	// 입력 불가

				ABPlayerController->GetHudWidget()->BindCharacterStat(stat);	// 캐릭터 스탯 컴포넌트와 HUD 위젯 연결

				auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
				ABCHECK(ABPlayerState != nullptr);
				stat->SetNewLevel(ABPlayerState->GetCharacterLevel());	// 해당 레벨의 데이터 테이블의 행 데이터로 캐릭터 스탯이 변경
			}

			else       // NPC이면
			{
				auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());				// 게임 모드 가져오기
				ABCHECK(ABGameMode != nullptr);
				int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));	// 소수점 첫자리 올림하여 정수 반환

				int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);	// TargetLevel을 1 ~ 20 사이의 값으로 한정지음
				ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
				stat->SetNewLevel(FinalLevel);
			}

			SetActorHiddenInGame(true);				// 캐릭터 안보이도록
			hpBarWidget->SetHiddenInGame(true);		// 체력바 UI 안보이도록
			SetCanBeDamaged(false);					// 데미지 입지 않도록
			break;
		}

		case ECharacterState::READY:
		{
			SetActorHiddenInGame(false);			// 캐릭터가 보이도록
			hpBarWidget->SetHiddenInGame(false);	// 체력바 UI도 보이도록
			SetCanBeDamaged(true);					// 대미지 입을 수 있음

			stat->OnHPIsZero.AddLambda([this]() -> void {		// 체력이 0이되면 DEAD 스테이트로 변경
					SetCharacterState(ECharacterState::DEAD);
			});

			auto CharacterWidget = Cast<UABCharacterWidget>(hpBarWidget->GetUserWidgetObject());
			ABCHECK(CharacterWidget != nullptr);

			// 체력바 UI와 캐릭터의 stat을 바인딩, 캐릭터의 현재 체력이 저장된 stat의 값을 UI의 프로그레스바에 전달
			CharacterWidget->BindCharacterStat(stat);	

			if (bIsPlayer)	// 플레이어이면
			{
				SetControlMode(EControlMode::DIABLO);
				GetCharacterMovement()->MaxWalkSpeed = 600.0f;
				EnableInput(ABPlayerController);

				// 캐릭터가 레벨업하면 체력회복
				// ABPlayerController에서 작성된 델리게이트(PlayerState의 AddExp에서 레벨업 했는지 True 전달받으면 실행됨.)
				ABPlayerController->OnLevelUP.AddLambda([this]() -> void {

					auto playerState = Cast<AABPlayerState>(GetPlayerState());
					ABCHECK(playerState != nullptr);

					stat->SetNewLevel(playerState->GetCharacterLevel());	// 해당 레벨의 데이터 테이블의 행 데이터로 캐릭터 스탯이 변경
				});
			}

			else			// NPC이면
			{
				SetControlMode(EControlMode::NPC);
				GetCharacterMovement()->MaxWalkSpeed = 400.0f;
				ABAIController->RunAI();
			}

			break;
		}

		case ECharacterState::DEAD:
		{
			SetActorEnableCollision(false);			// 충돌 콜리전 제거
			GetMesh()->SetHiddenInGame(false);		// 캐릭터가 보이도록
			hpBarWidget->SetHiddenInGame(true);		// 체력바 UI는 안보이도록
			ABAnim->SetDeadAnim();					// 사망 애니메이션 동작
			SetCanBeDamaged(false);					// 대미지 입지 않도록

			if (bIsPlayer)
			{
				DisableInput(ABPlayerController);		// 입력 불가상태
			}
			else
			{
				ABAIController->StopAI();	// 비헤이비어 트리 멈추기
			}

			// DeadTimer(= 5초) 후, 플레이어면 레벨 재시작, NPC면 액터 삭제
			GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
				if (bIsPlayer)
				{
					ABPlayerController->ShowResultUI();		// 결과 UI 띄우기
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

// NPC가 죽었을 때 주는 경험치
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

	// 몽타주가 끝나면 자동으로 실행
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// 위 OnMontageEnded 델리게이트가 실행될 때, OnAttackMontageEnded 함수가 실행되게 한것처럼
	// OnNextAttackCheck 델리게이트가 실행될 때, 호출될 함수를 람다식으로 작성
	// OnNextAttackCheckDelegate Broadcast 가 발생할때
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
	
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)	// 공격 버튼을 눌렀다면
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);	// 다음 몽타주 섹션(CurrentCombo)로 넘긴다.
		}
	});

	// 충돌 액터 탐지 델리게이트
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	// 체력이 0이 되었을 때 호출되는 델리게이트에 람다식으로 함수 구현
	stat->OnHPIsZero.AddLambda([this]()-> void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();				// isDead = true가 됨 -> 사망 애니메이션 동작
		SetActorEnableCollision(false);		// 충돌 설정 없앰
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

	// 삼인칭 모드 변경 함수(Shift + V)
	PlayerInputComponent->BindAction(TEXT("ViewChange"), IE_Pressed, this, &AABCharacter::ViewChange);
}


// 4개의 축 입력 매핑 함수
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

// 공격 입력 함수
void AABCharacter::Attack()
{
	if (IsAttacking) // 공격 중, 공격 버튼을 또 눌렀을 경우
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));

		if (CanNextCombo) {
			IsComboInputOn = true;	// NextAttackNotify에게 다음 공격 여부 true로 전달(= OnNextAttackCheck 델리게이트에 True가 확인되어 다음 공격 하게함)
		}
	}

	else   // 첫 공격일 때,
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();	// CurrentCombo + 1 
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);	// 섹션 변경
		IsAttacking = true;
	}
}

// 공격 몽타주 끝났다는 걸 나타내는 함수
void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

// 공격 시작한 상태
void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;																// CanNextCombo = 다음 콤보로 이동 가능 여부
	IsComboInputOn = false;																// IsComboInputOn = 콤보 입력 여부

	// CurrentCombo값이 범위 0 ~ 3 내부에 있는지 검사한다.
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));			// MaxCombo = 콤보 카운터의 최대치

	// CurrentCombo + 1한 값의 범위가 1 ~ MaxCombo(4)로 고정한다.
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);					// CurrentCombo = 현재 실행 중인 콤보 카운터
}

// 공격 애니메이션 종료
void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

// 공격했을 때, 액터의 충돌이 감지되면
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

	// 반지름 50, 탐색 시작 위치에서 탐색 끝 위치로 향하는 벡터를 구한 후, 벡터의 중점위치와 벡터 길이의 절반을 대입하면 원하는 크기의 캡슐 모양을 구할 수 있음
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;			// 앞으로 200cm거리 탐색
	FVector Center = GetActorLocation() + TraceVec * 0.5f;					// 벡터의 중점 위치
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;				// 벡터 길이의 절반
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();		// Z축 회전 좌표축 생성
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;				// 공격 범위 내 적이 탐색되면 녹색, 아니면 빨간색으로
	float DebugLifeTime = 5.0f;												// 캡슐 5초간 생성

	DrawDebugCapsule(GetWorld(),	// 캡슐 디버그 생성
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);


	// 비주얼 로거
	// 캐릭터의 위치는  50cm의 파란색 구체로 표현
	UE_VLOG_LOCATION(this, ArenaBattle, Verbose, GetActorLocation(), 50.0f, FColor::Blue, TEXT("Attack Position"));

	// 공격 판정 성공과 실패 구체 표현
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

// 액터에 대미지를 전달하는 TakeDamage 함수 오버라이드
float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// AttackCheck() 함수에서 HitResult.GetActor()의 TakeDamage() 함수가 호출된 것으로 GetName() 사용시 피해자 액터 이름이 출력됨.
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	stat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())	// 가해자가 플레이어인가 ?
		{
			auto controller = Cast<AABPlayerController>(EventInstigator);	// 가해자의 컨트롤러 가져오기
			ABCHECK(controller != nullptr, 0.0f);

			controller->NPCKill(this);	// 플레이어가 NPCKill 했다는 함수 호출 = NPCKill 함수는 플레이어의 경험치를 추가해주는 함수
		}
	}

	return FinalDamage;
}

// 무기를 얻을 수 있는지 확인하는 함수(항상 무기를 얻을 수 있도록 해줌)
bool AABCharacter::CanSetWeapon()
{
	return true;
}

// 무기를 장착하는 함수
void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(NewWeapon != nullptr);

	FName WeaponSocket(TEXT("hand_rSocket"));

	if (CurrentWeapon != nullptr)		// 현재 무기가 있다면
	{
		// 현재 연결된 컴포넌트에서 이 액터를 분리해낸다.
		// KeepWorldTransform는 월드 공간에서 변환된 변환을 통해 구성 요소를 분리해내는 것을 의미하는 것 같다.
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();	// 무기 제거
		CurrentWeapon = nullptr;	// 현재 무기 비어있음으로 만들기
	}

	if (NewWeapon != nullptr)	
	{
		// SnapToTargetNotIncludingScale : 주어진 상대적 또는 월드 트랜스폼의 스케일 매개변수를 무시한다.
		// 이렇게 하면 무기가 아웃 라이너에서 액터로 따로 분리되어 있다.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}


// 플레이어의 공격 범위 반환 
float AABCharacter::GetFinalAttackRange() const
{
	// 무기가 있으면 무기의 공격 범위를 반환, 무기가 없으면 맨손 상태의 공격 범위 반환
	return (CurrentWeapon != nullptr) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

// 최종 대미지 반환
float AABCharacter::GetFinalAttackDamage() const
{
	// 무기가 있으면 캐릭터 기본 공격력 + 무기 공격력 vs 무기가 없으면 캐릭터 기본 공격력 
	float AttackDamage = (CurrentWeapon != nullptr) ? (stat->GetAttack() + CurrentWeapon->GetAttackDamage()) : stat->GetAttack();

	// 무기가 있으면 무기의 효과치(몇배나 강하게) vs 무기가 없으면 기본 효과치 1 배수
	float AttackModifier = (CurrentWeapon != nullptr) ? CurrentWeapon->GetAttackModifier() : 1.0f;

	return AttackDamage * AttackModifier;
}

// 비동기로 애셋을 로딩하는 것이 끝나면 호출되는 함수
void AABCharacter::OnAssetLoadCompleted()
{
	// 스켈레탈 메시 경로에 있는 스켈레탈 메시를 가져옴
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	
	AssetStreamingHandle.Reset();
	ABCHECK(AssetLoaded != nullptr);
	
	GetMesh()->SetSkeletalMesh(AssetLoaded);	// 가져온 스켈레탈 메시로 적용

	SetCharacterState(ECharacterState::READY);	//	캐릭터 애셋 로딩이 끝났으므로, READY 상태로 전환
}


// 모드 변경 함수
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

// 삼인칭 모드 설정 함수
void AABCharacter::SetControlMode(EControlMode ControlMode)
{
	currentMode = ControlMode;

	switch (currentMode)
	{

		case EControlMode::GTA :
			// 마우스를 움직이면 캐릭터는 회전하지 않고, 가만히 서있음
			// 카메라 지지대가 마우스 움직임에 따라 회전
			//springArm->TargetArmLength = 450.0f;
			//springArm->SetRelativeRotation(FRotator::ZeroRotator);	// = FRotator(0, 0, 0);
			armLengthTo = 450.0f;
			springArm->bUsePawnControlRotation = true;				// 폰 제어 회전 사용(가능하면, 폰의 뷰/컨트롤러 회전 값을 사용할 것인지)
			springArm->bInheritPitch = true;						// 카메라 세팅(피치 상속)
			springArm->bInheritRoll = true;							// 카메라 세팅(롤 상속)	
			springArm->bInheritYaw = true;							// 카메라 세팅(요 상속)
			springArm->bDoCollisionTest = true;						// 콜리전 테스트 수행하여, 카메라 클리핑이 일어나는 것 방지
			bUseControllerRotationYaw = false;						// 컨트롤러 요 회전 끄기

			// 캐릭터 무브먼트 컴포넌트의 OrientRotationMovement 기능으로, 캐릭터가 움직이는 방향으로 캐릭터 자동 회전
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0, 720.0f, 0);	// 회전 속도
			break;

		case EControlMode::DIABLO :
			// 상하 키와 좌우 키를 조합하여 캐릭터 회전 및 이동
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

			// OrientRotationMovement 속성도 캐릭터가 움직이는 방향으로 캐릭터를 자동 회전시키지만, UseControllerDesiredRotation도 자동 회전 시켜준다.
			// UseControllerDesiredRotation 속성을 체크하면 컨트롤 회전이 가리키는 방향으로 캐릭터가 부드럽게 회전한다.
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
