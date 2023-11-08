// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UCLASS()
class GBGAME_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����


	// player�� �߰��Ͽ� ȸ���� �� ���ڿ��������� �ذ��ϱ� ���� ���
	// ���� ���� �Ǿ��� �� ����(�� �Լ��� READY ������Ʈ���� ����)
	virtual void PossessedBy(AController* newController) override;

	void Attack();			// ���� 
	void AttackCheck();		// ������ ��ȿ���� Ȯ���ϴ� �Լ�

	float GetHPRatio();		// ���� ü�� �ۼ�Ʈ ��ȯ
	void DeadEnemy();		// Enemy(= Morigesh)�� �׾��� �� ����� �Լ� 

	void FireBullet();			// BossBullet �����Լ�
	void FireHoming();			// BossHoming �����Լ�

	void SpawnLightning();		// Lightning ���� ����
	void FireLightning();		// Lightning ���� 5�� �������� �����ϰ�, ������ ���������ϴ� �Լ�
	void StopLightning();		// Lightning ���� ���� �Լ�

	void ReadyTornado();		// ����̵� ��ũ �����ϴ� �Լ�

	void MoveDown();			// Boss ��� ��, �Ʒ��� ������ + ������ �۾���
	void DeathOracleSpawn();	// Boss ��� ��, ����Ŭ ����

	//UPROPERTY(EditDefaultsOnly, Category = UI)
	//class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	class UGBHUDWidget* enemyHPWidget;

	//UPROPERTY(EditDefaultsOnly, Category = UI)
	//class UGBEnemyWidget* enemyWidget;

	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABossBullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABossHoming> homingFactory;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	FVector TargetLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float detectRange;		// Player Ž�� ����

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void animMontageSetting();			// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�
	void OnAssetLoadCompleted();		// Enemy �ּ� ���� �Ϸ� �Լ�

	UFUNCTION(BlueprintCallable, Category = "bullet")
	void HitBullet(class AGBCharacter* player);

	UFUNCTION(BlueprintCallable, Category = "homing")
	void HitHoming(class AGBCharacter* player);

	// Lightning ������ �ݸ����� �ε����� ��, ȣ��� �Լ�
	UFUNCTION(BlueprintCallable, Category = "lightning")
	void HitLightning(class AGBCharacter* player);

	// Lightning ������ �ݸ����� �ε����� ��, ȣ��� �Լ�
	UFUNCTION(BlueprintCallable, Category = "tornado")
	void HitTornado(class AGBCharacter* player);

	FSoftObjectPath enemyAssetToLoad = FSoftObjectPath(nullptr);	// �ּ� ���
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// �ּ� �񵿱�� �ε�

	UPROPERTY(EditDefaultsOnly, Category = GrapplingBox)
	class UBoxComponent* grapBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRange;		// ���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRadius;		// ���� �ݰ� ��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float maxHP;			// �ִ� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float currentHP;		// ���� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool isDead;			// enemy�� ����ִ��� ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FString enemyName;		// enemy �̸�

	UPROPERTY()
	class UBossAnimInstance* bossAnim;

	UPROPERTY()
	class AGBCharacter* GBCharacter;

 	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* hitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* attackMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* deathMontage;

	UPROPERTY()
	class AGBAIController* EnemyAIController;			// AI ��Ʈ�ѷ�

	FTimerHandle LightningTimerHandle;			// ���� ���� Ÿ�̸�
	float LightningSpawnInterval;				// �ð� ����

	FTimerHandle LightningDeleteTimerHandle;	// ���� ������ ���ߴ�Ÿ�̸�
	float LightningDeleteInterval;				// �ð� ����

	UPROPERTY()
	TArray<class ABossTornado*> arr_Tornado;

	UPROPERTY(VisibleAnywhere, Category = Tornado, Meta = (AllowPrivateAccess = true))
	int32 tornadoCnt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool isInTornado;			// ����̵��� ���ִ��� ?

	FTimerHandle TornadoTimerHandle;			// ���� ���� Ÿ�̸�
	FTimerHandle TornadoDeleteTimerHandle;		// ���� ������ ���ߴ�Ÿ�̸�
	float TornadoDeleteInterval;				// �ð� ����

	FTimerHandle MoveTimerHandle;				// ���� ���Ͱ� ������ �Ʒ��� ���������ϴ� Ÿ�̸�
	float alpha = 0.0f;							// Lerp �Լ��� ����� ���İ�

	UPROPERTY(EditDefaultsOnly, Category = Death, Meta = (AllowPrivateAccess = true))
	class ABossDeathOracle* DeathOracle;

	// FireBullet ���ƿ��� �Ҹ�
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* bulletSFX;

	// ���� �Ҹ�
	UPROPERTY(EditDefaultsOnly, Category = Death, Meta = (AllowPrivateAccess = true))
	class USoundBase* lightningSound;

	// Homing ��ü ���� SFX ȿ��
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* explosionSFX;

	// ȸ���� ���� SFX ȿ��
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* tornadoStartSFX;

	// ȸ���� ���� SFX ȿ��
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* tornadoSFX;
};
