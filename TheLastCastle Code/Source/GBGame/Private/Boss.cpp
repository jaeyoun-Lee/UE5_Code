// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "GBCharacter.h"
#include "BossAnimInstance.h"
#include "Engine/DamageEvents.h"	// FDamageEvent 선언 시, 사용(안쓰면 초기화 오류 발생)
#include "GBAIController.h"
#include "GBEnemySetting.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GBEnemyWidget.h"
#include "BossBullet.h"
#include "BossHoming.h"
#include "BossLightning.h"
#include "BossTornado.h"
#include "BossDeathOracle.h"
#include "GBGameState.h"
#include "GBGameInstance.h"
#include "GBPlayerController.h"
#include "GBHUDWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	grapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Grap Box"));
	grapBox->SetupAttachment(GetMesh());
	//HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP_Bar"));
	//HPBarWidget->SetupAttachment(GetMesh());
	

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Meshes/Sevarog.Sevarog'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> animIns(TEXT("/Script/Engine.AnimBlueprint'/Game/BP/Animation/Enemy_Sevarog/ABP_Sevarog.ABP_Sevarog_C'"));
	if (animIns.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animIns.Class);
	}

	static ConstructorHelpers::FClassFinder<ABossBullet> bullet(TEXT("/Script/CoreUObject.Class'/Script/GBGame.BossBullet'"));
	if (bullet.Succeeded())
	{
		bulletFactory = bullet.Class;
	}

	static ConstructorHelpers::FClassFinder<ABossHoming> homing(TEXT("/Script/CoreUObject.Class'/Script/GBGame.BossHoming'"));
	if (homing.Succeeded())
	{
		homingFactory = homing.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> bulletSoundEffect(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/BossSound/fireball-SFX_Cue.fireball-SFX_Cue'"));
	if (bulletSoundEffect.Succeeded())
	{
		bulletSFX = bulletSoundEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> lightningSFX(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/BossSound/Tunder_SFX1_Cue.Tunder_SFX1_Cue'"));
	if (lightningSFX.Succeeded())
	{
		lightningSound = lightningSFX.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> boomSFX(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/BossSound/Explosion01_Cue.Explosion01_Cue'"));
	if (boomSFX.Succeeded())
	{
		explosionSFX = boomSFX.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tornadoStart(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/BossSound/Wind-SFX1_Cue.Wind-SFX1_Cue'"));
	if (tornadoStart.Succeeded())
	{
		tornadoStartSFX = tornadoStart.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tornadoSound(TEXT("/Script/Engine.SoundCue'/Game/BP/Sound/BossSound/TornadoSFX1_Cue.TornadoSFX1_Cue'"));
	if (tornadoSound.Succeeded())
	{
		tornadoSFX = tornadoSound.Object;
	}

	animMontageSetting();	// 애님 몽타주 생성자에서 ConstructorHelpers 함수

	GetCapsuleComponent()->SetCapsuleHalfHeight(250.0f);
	GetCapsuleComponent()->SetCapsuleRadius(90.4f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -310.4f), FRotator(0, -100.0f, 0));
	GetMesh()->SetRelativeScale3D(FVector(2.5, 2.5, 2.5));

	grapBox->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	grapBox->SetRelativeLocationAndRotation(FVector(-4.56f, -4.86f, 149.2f),  FRotator(0, 100, 0));
	grapBox->SetRelativeScale3D(FVector(0.156f, 0.127f, 2.121f));
	grapBox->SetBoxExtent(FVector(426.239f, 445.9487f, 60.457f));

	// 캐릭터 움직임 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 480.0f, 0);

	AIControllerClass = AGBAIController::StaticClass();			// Enemy는 AI 컨트롤러에 조종되도록
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;		// Enemy 생성과 AI컨트롤러의 빙의 시점을 맵에 스폰된 후로 설정 + 캐릭터 수만큼 AI 컨트롤러 액터가 생성된다.

	attackRange = 300.0f;
	attackRadius = 220.0f;
	detectRange = 1900.0f;
	maxHP = 1000.0f;
	currentHP = maxHP;
	isDead = false;

	//HPBarWidget->SetRelativeLocation(FVector(0, 0, 345.0f));
	//HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);	// 항상 플레이어를 향해보도록

	//static ConstructorHelpers::FClassFinder<UUserWidget> hpBar(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_EnemyHP.UI_EnemyHP_C'"));
	//if (hpBar.Succeeded())
	//{
	//	HPBarWidget->SetWidgetClass(hpBar.Class);
	//	HPBarWidget->SetDrawSize(FVector2D(150.0f, 60.0f));
	//}

	FString skeletonName = GetMesh()->SkeletalMesh->Skeleton->GetName();
	TArray<FString> names;
	skeletonName.ParseIntoArray(names, TEXT("_"));
	enemyName = names[0];

	TargetLocation = FVector::ZeroVector;

	LightningSpawnInterval = 0.2f;   // 번개가 몇초마다 소환될 지
	LightningDeleteInterval = 1.0f;  // 몇초후 소환을 멈출지

	tornadoCnt = 5;
	TornadoDeleteInterval = 3.0f;
	isInTornado = false;
}


void ABoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bossAnim = Cast<UBossAnimInstance>(GetMesh()->GetAnimInstance());
	GBCHECK(bossAnim != nullptr);

	bossAnim->OnHitCheck.AddUObject(this, &ABoss::AttackCheck);
	bossAnim->OnEnemyDie.AddUObject(this, &ABoss::DeadEnemy);

	bossAnim->OnBulletSpawn.AddUObject(this, &ABoss::FireBullet);
	bossAnim->OnHomingSpawn.AddUObject(this, &ABoss::FireHoming);
	bossAnim->OnLightningSpawn.AddUObject(this, &ABoss::FireLightning);
	bossAnim->OnTornadoReady.AddUObject(this, &ABoss::ReadyTornado);			// 애님 몽타주의 노티파이를 지나면 토네이도 마크를 tornadoCnt만큼 소환하는 함수(소환될 위치 미리 보여주도록)

	// 토네이도 마크위치에 토네이도가 생성되도록하는 함수
	bossAnim->OnTornadoSpawn.AddLambda([this]() -> void {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), tornadoSFX, arr_Tornado[0]->GetActorLocation());

		for (int i = 0; i < arr_Tornado.Num(); i++)
		{
			ABossTornado* tornado = arr_Tornado[i];
			tornado->StartTornado();
		}

		// 토네이도가 생성된 후, 배열을 비우기
		arr_Tornado.Empty();	// 비우지 않으면 이전에 생성된 토네이도가 사라지기 전, 다른 토네이도 생성 시 그때 배열에 같이 존재하여 이전 토네이도가 생성된 위치에 한 번 더 생성됨		
	});
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	auto defaultSetting = GetDefault<UGBEnemySetting>();
	int32 randIndex = FMath::RandRange(0, defaultSetting->SevarogAssets.Num() - 1);
	enemyAssetToLoad = defaultSetting->SevarogAssets[randIndex];		// DefaultGBGame.ini에서 메시 경로 하나 가져옴

	auto GI = Cast<UGBGameInstance>(GetGameInstance());
	if (GI != nullptr)
	{
		// StreamableManager.RequestAsyncLoad 비동기로 애셋 로딩
		// FStreamableDelegate::CreateUObject로 즉석 델리게이트 생성하여 함수 호출
		AssetStreamingHandle = GI->streamableManager.RequestAsyncLoad(enemyAssetToLoad, FStreamableDelegate::CreateUObject(this, &ABoss::OnAssetLoadCompleted));
	}

	EnemyAIController = Cast<AGBAIController>(GetController());		// enemy의 AI 컨트롤러 가져오기
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->RunAI();								// Enemy가 비헤이비어 트리 동작하도록
	}

	auto userController = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (userController != nullptr)
	{
		enemyHPWidget = userController->GetHUDWidget();
		if (enemyHPWidget)
		{
			enemyHPWidget->UpdateBossHP(GetHPRatio());
		}
	}

	//enemyWidget = Cast<UGBEnemyWidget>(HPBarWidget->GetUserWidgetObject());
	//if (enemyWidget != nullptr)
	//{
	//	enemyWidget->UpdateHPWidget(GetHPRatio());
	//	enemyWidget->enemyText = enemyName;
	//}

	// Enemy가 체력이 0이되면 호출되는 델리게이트
	enemyHPWidget->OnDie.AddLambda([this]() -> void {
		GBLOG(Error, TEXT("Enemy is Dead....."));

		SetActorRotation(FRotator::ZeroRotator);
		SetActorEnableCollision(false);			// 충돌 설정 제거
		PlayAnimMontage(deathMontage, 1.0f);	// 사망 애니메이션 재생
		EnemyAIController->StopAI();			// AI 컨트롤러 제거
		isDead = true;							// 사망했다는 것을 알리는 변수 true로 변경
		DeathOracleSpawn();
	});
}


void ABoss::Attack()
{
	int32 sectionNum = FMath::RandRange(1, 7);				// 공격 몽타주 섹션이 7개임
	FName sectionName = FName(*FString::Printf(TEXT("Attack%d"), sectionNum));
	PlayAnimMontage(attackMontage, 1.0f, sectionName);
}


void ABoss::AttackCheck()
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

	FVector traceVec = GetActorForwardVector() * attackRange;			// 캐릭터의 시선방향 벡터
	FVector center = GetActorLocation() + traceVec * 0.5f;				// 캐릭터 현재 위치에서 벡터 중점 위치
	float halfHeight = attackRange * 0.5f + attackRadius;				// 캡슐 절반 높이
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

	FireBullet();

#endif		// ~#if ENABLE_DRAW_DEBUG  디버그 드로잉이 가능하게 하기위한 코드
	if (bResult)	// 충돌 감지되면(공격에 맞으면)
	{
		for (FHitResult result : OutHits)
		{
			auto player = Cast<AGBCharacter>(result.GetActor());
			if (player)
			{
				GBLOG(Warning, TEXT("Hit Actor Name : %s"), *result.GetActor()->GetName());
				float randDamage = 30 * (FMath::RandRange(0.5, 1.2));		// 공격 데미지 랜덤으로 설정

				FDamageEvent DamageEvent;		// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
				result.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()는 공격 당한 액터를 의미한다. 
			}
		}
	}
}

// 공격 당하여 받은 데미지 처리하는 함수
float ABoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// 살아있을때만 데미지를 받도록함
	if (!isDead)
	{
		GBLOG(Error, TEXT("Enemy TakeDamage"));
		float finalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		GBLOG(Warning, TEXT("Actor : %s, took Damage : %f"), *GetName(), finalDamage);
		currentHP -= finalDamage;

		if (!bossAnim->Montage_IsPlaying(attackMontage))
		{
			GBLOG(Warning, TEXT("Boss don't playing Attack"));
			PlayAnimMontage(hitReactMontage, 1.0f);
		}

		if (enemyHPWidget != nullptr)
		{
			enemyHPWidget->UpdateBossHP(GetHPRatio());
			// GBHUDWidget에서 보스 체력 업데이트시키기
		}

		return finalDamage;
	}
	return 0;
}

void ABoss::DeadEnemy()
{
	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ABoss::MoveDown, 0.01f, true);

	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));
	GBCHECK(gameState != nullptr);

	gameState->killedEnemy();
	GBLOG(Warning, TEXT("Numbers of Enemy = %d"), gameState->GetEnemyCount());
}

// 불꽃 투사체 발사
void ABoss::FireBullet()
{
	// 투사체가 생성될 위치 (Boss 무기에 소켓을 생성하고, 그 소켓의 위치로 지정)
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("FirePosition"));

	// 스폰될 월드 및 프로젝타일
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// 투사체를, 월드에서, 위에서 지정한 위치(= SpawnLocation)에서 회전 없이 생성한다.
		ABossBullet* bullet = World->SpawnActor<ABossBullet>(ABossBullet::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		
		// Bullet이 날라오는 효과음
		// PlaySound2D() 함수를 사용하면, 사운드 큐의 감쇠 없이 사운드가 재생되므로 UI에 사용하는 것이 좋다.
		UGameplayStatics::PlaySound2D(World, bulletSFX);

		// 생성된 투사체를 spawnLocation -> TargetLocation으로 이동
		// TargetLocation은 Boss.h에서 선언한 FVector 변수로 Target을 탐색하는 서비스 BTService_Detect에서 발견한 Player의 좌표를 매 초 전달하도록 했다.
		if (bullet)
		{
			TargetLocation.Z += 150.0f;
			bullet->FireFromTo(SpawnLocation, TargetLocation);
		}

		// 총알에 플레이어가 맞으면 HitBullet 함수 호출
		// OnAttackSucceess 델리게이트에서 맞은게 player라고 매개 변수로 받음
		bullet->OnBulletAttack.AddUObject(this, &ABoss::HitBullet);
	}
}

// OnAttackSucceess 델리게이트에서 호출하는 함수
// 델리게이트의 매개변수로 발사체에 맞은 player가 전달됨
void ABoss::HitBullet(AGBCharacter* player)
{
	float randDamage = 30 * (FMath::RandRange(0.5, 1.2));					// 공격 대미지 랜덤으로 설정

	FDamageEvent DamageEvent;												// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// 매개변수로 받은 player에게 대미지 전달
}


// Player를 따라가는 유도탄 발사
void ABoss::FireHoming()
{
	// 투사체가 생성될 위치 (Boss 무기에 소켓을 생성하고, 그 소켓의 위치로 지정)
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("HomingSpawn"));

	// 스폰될 월드 및 프로젝타일
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// 유도 투사체를, 월드에서, 위에서 지정한 위치(= SpawnLocation)에서 회전 없이 생성한다.
		ABossHoming* homing = World->SpawnActor<ABossHoming>(ABossHoming::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

		// 유도탄에 플레이어가 맞으면 HitHoming 함수 호출
		// OnHomingAttack 델리게이트에서 맞은게 player라고 매개 변수로 받음
		homing->OnHomingAttack.AddUObject(this, &ABoss::HitHoming);
	}
}

// OnHomingAttack 델리게이트에서 호출하는 함수
// 델리게이트의 매개변수로 발사체에 맞은 player가 전달됨
void ABoss::HitHoming(AGBCharacter* player)
{
	// PlaySound2D() 함수를 사용하면, 사운드 큐의 감쇠 없이 사운드가 재생되므로 UI에 사용하는 것이 좋다.
	UGameplayStatics::PlaySound2D(GetWorld(), explosionSFX);

	float randDamage = 30 * (FMath::RandRange(0.5, 1.2));					// 공격 대미지 랜덤으로 설정
	
	FDamageEvent DamageEvent;												// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// 매개변수로 받은 player에게 대미지 전달
}


// Lightning 나이아가라를 생성하는 함수
void ABoss::FireLightning()
{
	GetWorld()->GetTimerManager().SetTimer(LightningTimerHandle, this, &ABoss::SpawnLightning, LightningSpawnInterval, true, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(LightningDeleteTimerHandle, this, &ABoss::StopLightning, LightningDeleteInterval, false);

	// 낙뢰 소리 추가 
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), lightningSound, FVector(TargetLocation));
}


void ABoss::SpawnLightning()
{
	FVector2D RandXY = FMath::RandPointInCircle(300.0f);	// 지정된 반경 내에서 임의의 점 반환
	RandXY.X += 50;
	RandXY.Y += 50;
	
	// 스폰될 월드 및 프로젝타일
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// 유도 투사체를, 월드에서, 위에서 지정한 위치(= SpawnLocation)에서 회전 없이 생성한다.
		ABossLightning* lightning = World->SpawnActor<ABossLightning>(ABossLightning::StaticClass(), TargetLocation + FVector(RandXY, -800), FRotator::ZeroRotator, SpawnParams);


		// Lightning에 플레이어가 맞으면 HitLightning 함수 호출
		// OnLightningAttack 델리게이트에서 맞은게 player라고 매개 변수로 받음
		lightning->OnLightningAttack.AddUObject(this, &ABoss::HitLightning);
	}
}

// Lightning 나이아가라를 생성하는 함수 종료
void ABoss::StopLightning()
{
	GBLOG(Warning, TEXT("Stop the Lightning"));
	GetWorld()->GetTimerManager().ClearTimer(LightningTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LightningDeleteTimerHandle);
}


// OnLightningAttack 델리게이트에서 호출하는 함수 
// 델리게이트의 매개변수로 발사체에 맞은 player가 전달됨
void ABoss::HitLightning(AGBCharacter* player)
{
	float randDamage = 20 * (FMath::RandRange(0.5, 1.2));					// 공격 대미지 랜덤으로 설정

	FDamageEvent DamageEvent;												// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
	player->IsShocking = true;
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// 매개변수로 받은 player에게 대미지 전달
}

// 토네이도 마크 소환하는 함수
void ABoss::ReadyTornado()
{
	// PlaySoundAtLocation() 함수를 사용하면, 원하는 좌표에서 사운드 재생이 가능
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), tornadoStartSFX ,TargetLocation);

	// 토네이도 tornadoCnt 만큼 소환
	for (int i = 0; i < tornadoCnt; i++)
	{
		FVector2D RandXY = FMath::RandPointInCircle(600);	// 지정된 반경 내에서 임의의 점 반환
		//RandXY.X += 90.0f;								// 보스 몬스터위치에는 생성되지 않도록 X, Y 모두 90좌표씩 증가
		//RandXY.Y += 90.0f;

		// 스폰될 월드 및 프로젝타일
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			FVector tornadoSpawn = FVector(TargetLocation.X, TargetLocation.Y, 0) + FVector(RandXY, -100);
			//bossLocation.Z = 0;		 // 보스 Z좌표가 290이라서 토네이도가 공중에 생성되는 것을 막기위함

			// 유도 투사체를, 월드에서, 위에서 지정한 위치(= SpawnLocation)에서 회전 없이 생성한다.
			ABossTornado* tornadoMark = World->SpawnActor<ABossTornado>(ABossTornado::StaticClass(), tornadoSpawn, FRotator::ZeroRotator, SpawnParams);
			if (tornadoMark != nullptr)
			{
				tornadoMark->OnTornadoAttack.AddLambda([this](AGBCharacter* player) -> void {
					isInTornado = true;
					GBCharacter = Cast<AGBCharacter>(player);	
				});

				tornadoMark->OnTornadoAvoid.AddLambda([this](AGBCharacter* player) -> void {
					isInTornado = false;
					GBCharacter = nullptr;
				});

				GBLOG(Warning, TEXT("Tornado is Spawn"));
				arr_Tornado.Add(tornadoMark);
			}
		}
	}
}


// OnTornadoAttack 델리게이트에서 호출하는 함수 
// 델리게이트의 매개변수로 발사체에 맞은 player가 전달됨
void ABoss::HitTornado(AGBCharacter* player)
{
	float randDamage = 0.15;					// 공격 대미지 랜덤으로 설정

	if (player != nullptr)
	{
		FDamageEvent DamageEvent;												// 지역 변수 초기화 오류시, #include "Engine/DamageEvents.h" 헤더 파일 추가
		player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// 매개변수로 받은 player에게 대미지 전달
	}
}


// 보스 몬스터가 죽고, 아래로 사라지도록
void ABoss::MoveDown()
{
	alpha += GetWorld()->DeltaTimeSeconds;
	alpha = FMath::Clamp(alpha, 0.0, 1.0);

	FVector currentLocation = FMath::Lerp(GetActorLocation(), GetActorLocation() - FVector(0, 0, 100), alpha);
	this->SetActorLocation(currentLocation);

	FVector currentSize = FMath::Lerp(GetMesh()->GetRelativeScale3D(), FVector(0.7), alpha);
	GetMesh()->SetRelativeScale3D(currentSize);

	FVector oracleSize = FMath::Lerp(DeathOracle->GetActorRelativeScale3D(), FVector(0.0), alpha);
	DeathOracle->SetActorRelativeScale3D(oracleSize);

	if (alpha >= 1.0)	// 아래로 사라졌으면, Boss를 제거하고, 내려가게했던 타이머 제거
	{
		Destroy();
		DeathOracle->Destroy();
		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
}

// Boss 사망 시, 오라클 생성
void ABoss::DeathOracleSpawn()
{
	UWorld* World = GetWorld();

	if (World)
	{
		GBLOG(Warning, TEXT("Oracle is spawned"));
		FActorSpawnParameters SpawnParams;
		FVector spawnLocation = GetActorLocation();
		spawnLocation.Z = 0;

		DeathOracle = World->SpawnActor<ABossDeathOracle>(ABossDeathOracle::StaticClass(), spawnLocation, GetActorRotation(), SpawnParams);
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathOracle, spawnLocation, GetActorRotation(), FVector(2.0f));		// 파티클(= 캐스케이드) 스폰 시,
	}
}


float ABoss::GetHPRatio()
{
	return currentHP / maxHP;
}

// FStreamableDelegate::CreateUObjec로 즉석 델리게이트 생성하여 호출된 함수
// Enemy 스켈레탈 메시 랜덤으로 지정하는 함수
void ABoss::OnAssetLoadCompleted()
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
void ABoss::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);
}


// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isInTornado)
	{
		HitTornado(GBCharacter);
	}
}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// 애님 몽타주 생성자에서 ConstructorHelpers 함수
void ABoss::animMontageSetting()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> hitReact(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Sevarog/Hit_Sevarog.Hit_Sevarog'"));
	if (hitReact.Succeeded())
	{
		hitReactMontage = hitReact.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attack(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Sevarog/Attack_Sevarog.Attack_Sevarog'"));
	if (attack.Succeeded())
	{
		attackMontage = attack.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> death(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Sevarog/Death_Sevarog.Death_Sevarog'"));
	if (death.Succeeded())
	{
		deathMontage = death.Object;
	}
}

