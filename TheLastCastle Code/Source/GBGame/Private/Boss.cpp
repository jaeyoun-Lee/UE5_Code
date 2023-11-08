// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "GBCharacter.h"
#include "BossAnimInstance.h"
#include "Engine/DamageEvents.h"	// FDamageEvent ���� ��, ���(�Ⱦ��� �ʱ�ȭ ���� �߻�)
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

	animMontageSetting();	// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�

	GetCapsuleComponent()->SetCapsuleHalfHeight(250.0f);
	GetCapsuleComponent()->SetCapsuleRadius(90.4f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -310.4f), FRotator(0, -100.0f, 0));
	GetMesh()->SetRelativeScale3D(FVector(2.5, 2.5, 2.5));

	grapBox->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	grapBox->SetRelativeLocationAndRotation(FVector(-4.56f, -4.86f, 149.2f),  FRotator(0, 100, 0));
	grapBox->SetRelativeScale3D(FVector(0.156f, 0.127f, 2.121f));
	grapBox->SetBoxExtent(FVector(426.239f, 445.9487f, 60.457f));

	// ĳ���� ������ ����
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 480.0f, 0);

	AIControllerClass = AGBAIController::StaticClass();			// Enemy�� AI ��Ʈ�ѷ��� �����ǵ���
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;		// Enemy ������ AI��Ʈ�ѷ��� ���� ������ �ʿ� ������ �ķ� ���� + ĳ���� ����ŭ AI ��Ʈ�ѷ� ���Ͱ� �����ȴ�.

	attackRange = 300.0f;
	attackRadius = 220.0f;
	detectRange = 1900.0f;
	maxHP = 1000.0f;
	currentHP = maxHP;
	isDead = false;

	//HPBarWidget->SetRelativeLocation(FVector(0, 0, 345.0f));
	//HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);	// �׻� �÷��̾ ���غ�����

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

	LightningSpawnInterval = 0.2f;   // ������ ���ʸ��� ��ȯ�� ��
	LightningDeleteInterval = 1.0f;  // ������ ��ȯ�� ������

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
	bossAnim->OnTornadoReady.AddUObject(this, &ABoss::ReadyTornado);			// �ִ� ��Ÿ���� ��Ƽ���̸� ������ ����̵� ��ũ�� tornadoCnt��ŭ ��ȯ�ϴ� �Լ�(��ȯ�� ��ġ �̸� �����ֵ���)

	// ����̵� ��ũ��ġ�� ����̵��� �����ǵ����ϴ� �Լ�
	bossAnim->OnTornadoSpawn.AddLambda([this]() -> void {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), tornadoSFX, arr_Tornado[0]->GetActorLocation());

		for (int i = 0; i < arr_Tornado.Num(); i++)
		{
			ABossTornado* tornado = arr_Tornado[i];
			tornado->StartTornado();
		}

		// ����̵��� ������ ��, �迭�� ����
		arr_Tornado.Empty();	// ����� ������ ������ ������ ����̵��� ������� ��, �ٸ� ����̵� ���� �� �׶� �迭�� ���� �����Ͽ� ���� ����̵��� ������ ��ġ�� �� �� �� ������		
	});
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	auto defaultSetting = GetDefault<UGBEnemySetting>();
	int32 randIndex = FMath::RandRange(0, defaultSetting->SevarogAssets.Num() - 1);
	enemyAssetToLoad = defaultSetting->SevarogAssets[randIndex];		// DefaultGBGame.ini���� �޽� ��� �ϳ� ������

	auto GI = Cast<UGBGameInstance>(GetGameInstance());
	if (GI != nullptr)
	{
		// StreamableManager.RequestAsyncLoad �񵿱�� �ּ� �ε�
		// FStreamableDelegate::CreateUObject�� �Ｎ ��������Ʈ �����Ͽ� �Լ� ȣ��
		AssetStreamingHandle = GI->streamableManager.RequestAsyncLoad(enemyAssetToLoad, FStreamableDelegate::CreateUObject(this, &ABoss::OnAssetLoadCompleted));
	}

	EnemyAIController = Cast<AGBAIController>(GetController());		// enemy�� AI ��Ʈ�ѷ� ��������
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->RunAI();								// Enemy�� �����̺�� Ʈ�� �����ϵ���
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

	// Enemy�� ü���� 0�̵Ǹ� ȣ��Ǵ� ��������Ʈ
	enemyHPWidget->OnDie.AddLambda([this]() -> void {
		GBLOG(Error, TEXT("Enemy is Dead....."));

		SetActorRotation(FRotator::ZeroRotator);
		SetActorEnableCollision(false);			// �浹 ���� ����
		PlayAnimMontage(deathMontage, 1.0f);	// ��� �ִϸ��̼� ���
		EnemyAIController->StopAI();			// AI ��Ʈ�ѷ� ����
		isDead = true;							// ����ߴٴ� ���� �˸��� ���� true�� ����
		DeathOracleSpawn();
	});
}


void ABoss::Attack()
{
	int32 sectionNum = FMath::RandRange(1, 7);				// ���� ��Ÿ�� ������ 7����
	FName sectionName = FName(*FString::Printf(TEXT("Attack%d"), sectionNum));
	PlayAnimMontage(attackMontage, 1.0f, sectionName);
}


void ABoss::AttackCheck()
{
	TArray< struct FHitResult> OutHits;						// ������ �浹�� Ž���� ���, ���õ� ������ ���� ����ü �迭
	FCollisionQueryParams params(NAME_None, false, this);	// Ž�� ����� ���� ���� ���� ��Ƶ� ����ü

	bool bResult = GetWorld()->SweepMultiByChannel(			// Trace ä�η� ������ �浹 ���� Ȯ��(�浹�� ��ü ��� ������= SweepMultiByChannel)
		OutHits,
		GetActorLocation(),											// ���� ĳ���� ��ġ���� �����Ͽ�
		GetActorLocation() + GetActorForwardVector() * attackRange,	// ���� ĳ���� ���� 200cm Ž��
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,					// Attack Ʈ���̽� ���
		FCollisionShape::MakeSphere(attackRadius),					// ������ 50cm�� ��ü�� Ž���Ѵ�.
		params);

#if ENABLE_DRAW_DEBUG	// ����� ������� �����ϰ� �ϱ����� �ڵ�

	FVector traceVec = GetActorForwardVector() * attackRange;			// ĳ������ �ü����� ����
	FVector center = GetActorLocation() + traceVec * 0.5f;				// ĳ���� ���� ��ġ���� ���� ���� ��ġ
	float halfHeight = attackRange * 0.5f + attackRadius;				// ĸ�� ���� ����
	FQuat capsuleRot = FRotationMatrix::MakeFromZ(traceVec).ToQuat();	// ĸ���� �׸������� �ʿ��� ȸ�� ��ǥ�� 
	FColor drawColor = bResult ? FColor::Blue : FColor::Orange;			// ���� ���� ��, �ʷ���, ���� ���� ��, �������� ����� ��ο�
	float debugLifeTime = 5.0f;											// ����� ��ο�(ĸ�� ���) ���� �ð�

	DrawDebugCapsule(GetWorld(),
		center,
		halfHeight,
		attackRadius,
		capsuleRot,
		drawColor,
		false,
		debugLifeTime);

	FireBullet();

#endif		// ~#if ENABLE_DRAW_DEBUG  ����� ������� �����ϰ� �ϱ����� �ڵ�
	if (bResult)	// �浹 �����Ǹ�(���ݿ� ������)
	{
		for (FHitResult result : OutHits)
		{
			auto player = Cast<AGBCharacter>(result.GetActor());
			if (player)
			{
				GBLOG(Warning, TEXT("Hit Actor Name : %s"), *result.GetActor()->GetName());
				float randDamage = 30 * (FMath::RandRange(0.5, 1.2));		// ���� ������ �������� ����

				FDamageEvent DamageEvent;		// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
				result.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()�� ���� ���� ���͸� �ǹ��Ѵ�. 
			}
		}
	}
}

// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
float ABoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// ����������� �������� �޵�����
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
			// GBHUDWidget���� ���� ü�� ������Ʈ��Ű��
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

// �Ҳ� ����ü �߻�
void ABoss::FireBullet()
{
	// ����ü�� ������ ��ġ (Boss ���⿡ ������ �����ϰ�, �� ������ ��ġ�� ����)
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("FirePosition"));

	// ������ ���� �� ������Ÿ��
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// ����ü��, ���忡��, ������ ������ ��ġ(= SpawnLocation)���� ȸ�� ���� �����Ѵ�.
		ABossBullet* bullet = World->SpawnActor<ABossBullet>(ABossBullet::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		
		// Bullet�� ������� ȿ����
		// PlaySound2D() �Լ��� ����ϸ�, ���� ť�� ���� ���� ���尡 ����ǹǷ� UI�� ����ϴ� ���� ����.
		UGameplayStatics::PlaySound2D(World, bulletSFX);

		// ������ ����ü�� spawnLocation -> TargetLocation���� �̵�
		// TargetLocation�� Boss.h���� ������ FVector ������ Target�� Ž���ϴ� ���� BTService_Detect���� �߰��� Player�� ��ǥ�� �� �� �����ϵ��� �ߴ�.
		if (bullet)
		{
			TargetLocation.Z += 150.0f;
			bullet->FireFromTo(SpawnLocation, TargetLocation);
		}

		// �Ѿ˿� �÷��̾ ������ HitBullet �Լ� ȣ��
		// OnAttackSucceess ��������Ʈ���� ������ player��� �Ű� ������ ����
		bullet->OnBulletAttack.AddUObject(this, &ABoss::HitBullet);
	}
}

// OnAttackSucceess ��������Ʈ���� ȣ���ϴ� �Լ�
// ��������Ʈ�� �Ű������� �߻�ü�� ���� player�� ���޵�
void ABoss::HitBullet(AGBCharacter* player)
{
	float randDamage = 30 * (FMath::RandRange(0.5, 1.2));					// ���� ����� �������� ����

	FDamageEvent DamageEvent;												// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// �Ű������� ���� player���� ����� ����
}


// Player�� ���󰡴� ����ź �߻�
void ABoss::FireHoming()
{
	// ����ü�� ������ ��ġ (Boss ���⿡ ������ �����ϰ�, �� ������ ��ġ�� ����)
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("HomingSpawn"));

	// ������ ���� �� ������Ÿ��
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// ���� ����ü��, ���忡��, ������ ������ ��ġ(= SpawnLocation)���� ȸ�� ���� �����Ѵ�.
		ABossHoming* homing = World->SpawnActor<ABossHoming>(ABossHoming::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

		// ����ź�� �÷��̾ ������ HitHoming �Լ� ȣ��
		// OnHomingAttack ��������Ʈ���� ������ player��� �Ű� ������ ����
		homing->OnHomingAttack.AddUObject(this, &ABoss::HitHoming);
	}
}

// OnHomingAttack ��������Ʈ���� ȣ���ϴ� �Լ�
// ��������Ʈ�� �Ű������� �߻�ü�� ���� player�� ���޵�
void ABoss::HitHoming(AGBCharacter* player)
{
	// PlaySound2D() �Լ��� ����ϸ�, ���� ť�� ���� ���� ���尡 ����ǹǷ� UI�� ����ϴ� ���� ����.
	UGameplayStatics::PlaySound2D(GetWorld(), explosionSFX);

	float randDamage = 30 * (FMath::RandRange(0.5, 1.2));					// ���� ����� �������� ����
	
	FDamageEvent DamageEvent;												// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// �Ű������� ���� player���� ����� ����
}


// Lightning ���̾ư��� �����ϴ� �Լ�
void ABoss::FireLightning()
{
	GetWorld()->GetTimerManager().SetTimer(LightningTimerHandle, this, &ABoss::SpawnLightning, LightningSpawnInterval, true, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(LightningDeleteTimerHandle, this, &ABoss::StopLightning, LightningDeleteInterval, false);

	// ���� �Ҹ� �߰� 
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), lightningSound, FVector(TargetLocation));
}


void ABoss::SpawnLightning()
{
	FVector2D RandXY = FMath::RandPointInCircle(300.0f);	// ������ �ݰ� ������ ������ �� ��ȯ
	RandXY.X += 50;
	RandXY.Y += 50;
	
	// ������ ���� �� ������Ÿ��
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		// ���� ����ü��, ���忡��, ������ ������ ��ġ(= SpawnLocation)���� ȸ�� ���� �����Ѵ�.
		ABossLightning* lightning = World->SpawnActor<ABossLightning>(ABossLightning::StaticClass(), TargetLocation + FVector(RandXY, -800), FRotator::ZeroRotator, SpawnParams);


		// Lightning�� �÷��̾ ������ HitLightning �Լ� ȣ��
		// OnLightningAttack ��������Ʈ���� ������ player��� �Ű� ������ ����
		lightning->OnLightningAttack.AddUObject(this, &ABoss::HitLightning);
	}
}

// Lightning ���̾ư��� �����ϴ� �Լ� ����
void ABoss::StopLightning()
{
	GBLOG(Warning, TEXT("Stop the Lightning"));
	GetWorld()->GetTimerManager().ClearTimer(LightningTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LightningDeleteTimerHandle);
}


// OnLightningAttack ��������Ʈ���� ȣ���ϴ� �Լ� 
// ��������Ʈ�� �Ű������� �߻�ü�� ���� player�� ���޵�
void ABoss::HitLightning(AGBCharacter* player)
{
	float randDamage = 20 * (FMath::RandRange(0.5, 1.2));					// ���� ����� �������� ����

	FDamageEvent DamageEvent;												// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
	player->IsShocking = true;
	player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// �Ű������� ���� player���� ����� ����
}

// ����̵� ��ũ ��ȯ�ϴ� �Լ�
void ABoss::ReadyTornado()
{
	// PlaySoundAtLocation() �Լ��� ����ϸ�, ���ϴ� ��ǥ���� ���� ����� ����
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), tornadoStartSFX ,TargetLocation);

	// ����̵� tornadoCnt ��ŭ ��ȯ
	for (int i = 0; i < tornadoCnt; i++)
	{
		FVector2D RandXY = FMath::RandPointInCircle(600);	// ������ �ݰ� ������ ������ �� ��ȯ
		//RandXY.X += 90.0f;								// ���� ������ġ���� �������� �ʵ��� X, Y ��� 90��ǥ�� ����
		//RandXY.Y += 90.0f;

		// ������ ���� �� ������Ÿ��
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			FVector tornadoSpawn = FVector(TargetLocation.X, TargetLocation.Y, 0) + FVector(RandXY, -100);
			//bossLocation.Z = 0;		 // ���� Z��ǥ�� 290�̶� ����̵��� ���߿� �����Ǵ� ���� ��������

			// ���� ����ü��, ���忡��, ������ ������ ��ġ(= SpawnLocation)���� ȸ�� ���� �����Ѵ�.
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


// OnTornadoAttack ��������Ʈ���� ȣ���ϴ� �Լ� 
// ��������Ʈ�� �Ű������� �߻�ü�� ���� player�� ���޵�
void ABoss::HitTornado(AGBCharacter* player)
{
	float randDamage = 0.15;					// ���� ����� �������� ����

	if (player != nullptr)
	{
		FDamageEvent DamageEvent;												// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
		player->TakeDamage(randDamage, DamageEvent, GetController(), this);		// �Ű������� ���� player���� ����� ����
	}
}


// ���� ���Ͱ� �װ�, �Ʒ��� ���������
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

	if (alpha >= 1.0)	// �Ʒ��� ���������, Boss�� �����ϰ�, ���������ߴ� Ÿ�̸� ����
	{
		Destroy();
		DeathOracle->Destroy();
		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
}

// Boss ��� ��, ����Ŭ ����
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
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathOracle, spawnLocation, GetActorRotation(), FVector(2.0f));		// ��ƼŬ(= ĳ�����̵�) ���� ��,
	}
}


float ABoss::GetHPRatio()
{
	return currentHP / maxHP;
}

// FStreamableDelegate::CreateUObjec�� �Ｎ ��������Ʈ �����Ͽ� ȣ��� �Լ�
// Enemy ���̷�Ż �޽� �������� �����ϴ� �Լ�
void ABoss::OnAssetLoadCompleted()
{
	USkeletalMesh* assetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();

	if (assetLoaded != nullptr)
	{
		GetMesh()->SetSkeletalMesh(assetLoaded);
	}
}


// player�� �߰��Ͽ� ȸ���� �� ���ڿ��������� �ذ��ϱ� ���� ���
// ���� ���� �Ǿ��� �� ����(�� �Լ��� READY ������Ʈ���� ����)
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


// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�
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

