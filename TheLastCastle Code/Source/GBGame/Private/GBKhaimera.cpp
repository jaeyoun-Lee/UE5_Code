// Fill out your copyright notice in the Description page of Project Settings.


#include "GBKhaimera.h"
#include "GBCharacter.h"
#include "KhaimeraAnimInstance.h"
#include "Engine/DamageEvents.h"	// FDamageEvent ���� ��, ���(�Ⱦ��� �ʱ�ȭ ���� �߻�)
#include "GBAIController.h"
#include "GBEnemySetting.h"
#include "Components/WidgetComponent.h"
#include "GBEnemyWidget.h"
#include "GBGameState.h"
#include "GBGameInstance.h"

// Sets default values
AGBKhaimera::AGBKhaimera()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP_Bar"));
	HPBarWidget->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKhaimera/Characters/Heroes/Khaimera/Meshes/Khaimera.Khaimera'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/BP/Animation/Enemy_Kaimera/ABP_Kaimera.ABP_Kaimera_C'"));
	if (animInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animInstance.Class);
	}

	animMontageSetting();	// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�

	GetCapsuleComponent()->SetCapsuleHalfHeight(110.0);
	GetCapsuleComponent()->SetCapsuleRadius(45.4f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GBCharacter"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -111.0f), FRotator(0, -90.0f, 0));

	// ĳ���� ������ ����
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 480.0f, 0);

	AIControllerClass = AGBAIController::StaticClass();			// Enemy�� AI ��Ʈ�ѷ��� �����ǵ���
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;		// Enemy ������ AI��Ʈ�ѷ��� ���� ������ �ʿ� ������ �ķ� ���� + ĳ���� ����ŭ AI ��Ʈ�ѷ� ���Ͱ� �����ȴ�.

	attackRange = 150.0f;
	attackRadius = 50.0f;
	maxHP = 150.0f;
	currentHP = maxHP;
	isDead = false;

	HPBarWidget->SetRelativeLocation(FVector(0, 0, 230.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);	// �׻� �÷��̾ ���غ�����

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

void AGBKhaimera::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	khaimeraAnim = Cast<UKhaimeraAnimInstance>(GetMesh()->GetAnimInstance());
	GBCHECK(khaimeraAnim != nullptr);

	khaimeraAnim->OnHitCheck.AddUObject(this, &AGBKhaimera::AttackCheck);
	khaimeraAnim->OnEnemyDie.AddUObject(this, &AGBKhaimera::DeadEnemy);
}

// Called when the game starts or when spawned
void AGBKhaimera::BeginPlay()
{
	Super::BeginPlay();

	auto defaultSetting = GetDefault<UGBEnemySetting>();
	int32 randIndex = FMath::RandRange(0, defaultSetting->KhaimeraAssets.Num() - 1);
	enemyAssetToLoad = defaultSetting->KhaimeraAssets[randIndex];		// DefaultGBGame.ini���� �޽� ��� �ϳ� ������

	auto GI = Cast<UGBGameInstance>(GetGameInstance());
	if (GI != nullptr)
	{
		// StreamableManager.RequestAsyncLoad �񵿱�� �ּ� �ε�
		// FStreamableDelegate::CreateUObject�� �Ｎ ��������Ʈ �����Ͽ� �Լ� ȣ��
		AssetStreamingHandle = GI->streamableManager.RequestAsyncLoad(enemyAssetToLoad, FStreamableDelegate::CreateUObject(this, &AGBKhaimera::OnAssetLoadCompleted));
	}

	EnemyAIController = Cast<AGBAIController>(GetController());		// enemy�� AI ��Ʈ�ѷ� ��������
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->RunAI();								// Enemy�� �����̺�� Ʈ�� �����ϵ���
	}

	enemyWidget = Cast<UGBEnemyWidget>(HPBarWidget->GetUserWidgetObject());
	if (enemyWidget != nullptr)
	{
		enemyWidget->UpdateHPWidget(GetHPRatio());
		enemyWidget->enemyText = enemyName;

		enemyWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	
	// Enemy�� ü���� 0�̵Ǹ� ȣ��Ǵ� ��������Ʈ
	enemyWidget->OnDie.AddLambda([this]() -> void {
		GBLOG(Error, TEXT("Enemy is Dead....."));

		SetActorEnableCollision(false);			// �浹 ���� ����
		HPBarWidget->SetHiddenInGame(true);		// ü�¹� UI �����
		PlayAnimMontage(deathMontage, 1.0f);
		EnemyAIController->StopAI();
		isDead = true;
	});
}


void AGBKhaimera::Attack()
{
	int32 sectionNum = FMath::RandRange(1, 3);
	FName sectionName = FName(*FString::Printf(TEXT("Attack%d"), sectionNum));
	PlayAnimMontage(attackMontage, 1.0f, sectionName);
}


void AGBKhaimera::AttackCheck()
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

	FVector traceVec = GetActorForwardVector() * attackRange;		// ĳ������ �ü����� ����
	FVector center = GetActorLocation() + traceVec * 0.5f;			// ĳ���� ���� ��ġ���� ���� ���� ��ġ
	float halfHeight = attackRange * 0.5f + attackRadius;			// ĸ�� ���� ����
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

#endif		// ~#if ENABLE_DRAW_DEBUG  ����� ������� �����ϰ� �ϱ����� �ڵ�

	if (bResult)	// �浹 �����Ǹ�(���ݿ� ������)
	{
		for (FHitResult result : OutHits)
		{
			auto player = Cast<AGBCharacter>(result.GetActor());
			if (player)
			{
				GBLOG(Warning, TEXT("Hit Actor Name : %s"), *result.GetActor()->GetName());
				float randDamage = 15 * (FMath::RandRange(0.5, 1.2));		// ���� ������ �������� ����

				FDamageEvent DamageEvent;		// ���� ���� �ʱ�ȭ ������, #include "Engine/DamageEvents.h" ��� ���� �߰�
				result.GetActor()->TakeDamage(randDamage, DamageEvent, GetController(), this);	// hitResult.GetActor()�� ���� ���� ���͸� �ǹ��Ѵ�. 
			}
		}
	}
}

// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
float AGBKhaimera::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// ����������� �������� �޵�����
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


void AGBKhaimera::DeadEnemy()
{
	Destroy();
	auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));
	GBCHECK(gameState != nullptr);

	gameState->killedEnemy();
	GBLOG(Warning, TEXT("Numbers of Enemy = %d"), gameState->GetEnemyCount());
}

// FStreamableDelegate::CreateUObjec�� �Ｎ ��������Ʈ �����Ͽ� ȣ��� �Լ�
// Enemy ���̷�Ż �޽� �������� �����ϴ� �Լ�
void AGBKhaimera::OnAssetLoadCompleted()
{
	USkeletalMesh* assetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();

	if (assetLoaded != nullptr)
	{
		GetMesh()->SetSkeletalMesh(assetLoaded);
	}
}

float AGBKhaimera::GetHPRatio()
{
	return currentHP / maxHP;
}

// player�� �߰��Ͽ� ȸ���� �� ���ڿ��������� �ذ��ϱ� ���� ���
// ���� ���� �Ǿ��� �� ����(�� �Լ��� READY ������Ʈ���� ����)
void AGBKhaimera::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);
}


// Called every frame
void AGBKhaimera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGBKhaimera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�
void AGBKhaimera::animMontageSetting()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> hitReact(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Kaimera/HitReact_Montage.HitReact_Montage'"));
	if (hitReact.Succeeded())
	{
		hitReactMontage = hitReact.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attack(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Kaimera/AttackMontage.AttackMontage'"));
	if (attack.Succeeded())
	{
		attackMontage = attack.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> death(TEXT("/Script/Engine.AnimMontage'/Game/BP/Animation/Enemy_Kaimera/Death.Death'"));
	if (death.Succeeded())
	{
		deathMontage = death.Object;
	}
}