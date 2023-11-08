// Fill out your copyright notice in the Description page of Project Settings.


#include "GBEnemy.h"
#include "GBCharacter.h"
#include "GBEnemyAnimInstance.h"
#include "Engine/DamageEvents.h"	// FDamageEvent 선언 시, 사용(안쓰면 초기화 오류 발생)
#include "GBAIController.h"
#include "GBEnemySetting.h"
#include "Components/WidgetComponent.h"
#include "GBEnemyWidget.h"
#include "GBGameState.h"
#include "GBGameInstance.h"

// Sets default values
AGBEnemy::AGBEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP_Bar"));

	HPBarWidget->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Skins/Tier_1_5/Chestplate/Meshes/GruxChestplate.GruxChestplate'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/BP/Animation/Enemy_Grux/ABP_GBEnemy.ABP_GBEnemy_C'"));
	if (animInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animInstance.Class);
	}

	animMontageSetting();	// 애님 몽타주 생성자에서 ConstructorHelpers 함수

	GetCapsuleComponent()->SetCapsuleHalfHeight(116.0f);
	GetCapsuleComponent()->SetCapsuleRadius(45.4f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -117), FRotator(0, -90.0f, 0));

	// 캐릭터 움직임 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 480.0f, 0);

	AIControllerClass = AGBAIController::StaticClass();			// Enemy는 AI 컨트롤러에 조종되도록
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;		// Enemy 생성과 AI컨트롤러의 빙의 시점을 맵에 스폰된 후로 설정 + 캐릭터 수만큼 AI 컨트롤러 액터가 생성된다.

	attackRange = 150.0f;
	attackRadius = 50.0f;
	maxHP = 150.0f;
	currentHP = maxHP;
	isDead = false;

	HPBarWidget->SetRelativeLocation(FVector(0, 0, 250.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);	// 항상 플레이어를 향해보도록
	
	static ConstructorHelpers::FClassFinder<UUserWidget> hpBar(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_EnemyHP.UI_EnemyHP_C'"));
	if (hpBar.Succeeded())
	{
		HPBarWidget->SetWidgetClass(hpBar.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 60.0f));
	}

	FString skeletonName = GetMesh()->SkeletalMesh->Skeleton->GetName();
	TArray<FString> names;
	skeletonName.ParseIntoArray(names, TEXT("_"));
	enemyName = names[0];
}

void AGBEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	EnemyAnim = Cast<UGBEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	GBCHECK(EnemyAnim != nullptr);

	EnemyAnim->OnHitCheck.AddUObject(this, &AGBEnemy::AttackCheck);
	EnemyAnim->OnEnemyDie.AddUObject(this, &AGBEnemy::DeadEnemy);
}


// Called when the game starts or when spawned
void AGBEnemy::BeginPlay()
{
	Super::BeginPlay();

	auto defaultSetting = GetDefault<UGBEnemySetting>();
	int32 randIndex = FMath::RandRange(0, defaultSetting->CharacterAssets.Num() - 1);	
	enemyAssetToLoad = defaultSetting->CharacterAssets[randIndex];		// DefaultGBGame.ini에서 메시 경로 하나 가져옴

	auto GI = Cast<UGBGameInstance>(GetGameInstance());
	if (GI != nullptr) 
	{
		// StreamableManager.RequestAsyncLoad 비동기로 애셋 로딩
		// FStreamableDelegate::CreateUObject로 즉석 델리게이트 생성하여 함수 호출
		AssetStreamingHandle = GI->streamableManager.RequestAsyncLoad(enemyAssetToLoad, FStreamableDelegate::CreateUObject(this, &AGBEnemy::OnAssetLoadCompleted));
	}

	EnemyAIController = Cast<AGBAIController>(GetController());		// enemy의 AI 컨트롤러 가져오기
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->RunAI();								// Enemy가 비헤이비어 트리 동작하도록
	}

	enemyWidget = Cast<UGBEnemyWidget>(HPBarWidget->GetUserWidgetObject());
	if (enemyWidget != nullptr)
	{
		enemyWidget->UpdateHPWidget(GetHPRatio());
		enemyWidget->enemyText = enemyName;

		enemyWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	// Enemy가 체력이 0이되면 호출되는 델리게이트
	enemyWidget->OnDie.AddLambda([this]() -> void {
		GBLOG(Error, TEXT("Enemy is Dead....."));

		SetActorEnableCollision(false);			// 충돌 설정 제거
		HPBarWidget->SetHiddenInGame(true);		// 체력바 UI 숨기기
		PlayAnimMontage(deathMontage, 1.0f);	// 사망 애니메이션 재생
		EnemyAIController->StopAI();			// 비헤이비어 트리 멈추기
		isDead = true;							// 사망했다고 변수 변경
	});
}


void AGBEnemy::Attack()
{
	int32 sectionNum = FMath::RandRange(1, 3);
	FName sectionName = FName(*FString::Printf(TEXT("Attack%d"), sectionNum));
	PlayAnimMontage(attackMontage, 1.0f, sectionName);
}


void AGBEnemy::AttackCheck()
{
	TArray< struct FHitResult> OutHits;						// 물리적 충돌이 탐지된 경우, 관련된 정보를 담을 구조체 배열
	FCollisionQueryParams params(NAME_None, false, this);	// 탐색 방법에 대한 설정 값을 모아둔 구조체

	bool bResult = GetWorld()->SweepMultiByChannel(			// Trace 채널로 물리적 충돌 여부 확인(충돌된 물체 모두 가져옴= SweepMultiByChannel)
		OutHits,
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
	FColor drawColor = bResult ? FColor::Blue : FColor::Orange;			// 공격 성공 시, 초록을, 공격 실패 시, 빨강으로 디버깅 드로우
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
		for (FHitResult result : OutHits)
		{
			auto player = Cast<AGBCharacter>(result.GetActor());
			if (player)
			{
				GBLOG(Warning, TEXT("Hit Actor Name : %s"), *result.GetActor()->GetName());
				float randDamage = 15 * (FMath::RandRange(0.5, 1.2));		// 공격 데미지 랜덤으로 설정

				FDamageEvent DamageEvent;		// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
				result.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()는 공격 당한 액터를 의미한다. 
			}
		}
	}
}

// 공격 당하여 받은 데미지 처리하는 함수
float AGBEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// 살아있을때만 데미지를 받도록함
	if (!isDead)
	{
		if (enemyWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			enemyWidget->SetVisibility(ESlateVisibility::Visible);
		}

		GBLOG(Error, TEXT("Enemy TakeDamage"));
		float finalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		GBLOG(Warning, TEXT("Actor : %s, took Damage : %f"), *GetName(), finalDamage);
		currentHP -= finalDamage;
		PlayAnimMontage(hitReactMontage, 1.0f);

		if (enemyWidget != nullptr)
		{
			enemyWidget->UpdateHPWidget(GetHPRatio());
		}

		return finalDamage;
	}
	return 0;
}

// Enemy 사망시 호출되는 델리게이트에 바인딩 되어있는 함수
void AGBEnemy::DeadEnemy()
{
	Destroy();																	// 액터 제거
	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));	// 게임 스테이트 가져오기
	GBCHECK(gameState != nullptr);

	gameState->killedEnemy();													// Enemy 죽으면 호출되는 함수(현재 Enemy수 - 1)
	GBLOG(Warning, TEXT("Numbers of Enemy = %d"), gameState->GetEnemyCount());	// 현재 남아있는 Enemy 수 반환(로그에 출력)
}

float AGBEnemy::GetHPRatio()
{
	return currentHP / maxHP;
}

// FStreamableDelegate::CreateUObjec로 즉석 델리게이트 생성하여 호출된 함수
// Enemy 스켈레탈 메시 랜덤으로 지정하는 함수
void AGBEnemy::OnAssetLoadCompleted()
{
	USkeletalMesh* assetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();

	if (assetLoaded != nullptr)
	{
		GetMesh()->SetSkeletalMesh(assetLoaded);
	}
}


// player를 발견하여 회전할 때 부자연스러움을 해결하기 위해 사용
// 폰이 빙의 되었을 때 실행(이 함수는 READY 스테이트에서 구현)
void AGBEnemy::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);
}


// Called every frame
void AGBEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGBEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// 애님 몽타주 생성자에서 ConstructorHelpers 함수
void AGBEnemy::animMontageSetting()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> hitReact(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Grux/HitReact_Front_Montage.HitReact_Front_Montage'"));
	if (hitReact.Succeeded())
	{
		hitReactMontage = hitReact.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attack(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Grux/PrimaryAttack_LA_Montage.PrimaryAttack_LA_Montage'"));
	if (attack.Succeeded())
	{
		attackMontage = attack.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> death(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Grux/Grux_Death.Grux_Death'"));
	if (death.Succeeded())
	{
		deathMontage = death.Object;
	}
}