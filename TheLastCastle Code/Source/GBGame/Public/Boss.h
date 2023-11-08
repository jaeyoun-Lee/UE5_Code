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

	// 공격 당하여 받은 데미지 처리하는 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;	// 폰과 플레이어 컨트롤러가 생성되는 시점


	// player를 발견하여 회전할 때 부자연스러움을 해결하기 위해 사용
	// 폰이 빙의 되었을 때 실행(이 함수는 READY 스테이트에서 구현)
	virtual void PossessedBy(AController* newController) override;

	void Attack();			// 공격 
	void AttackCheck();		// 공격이 유효한지 확인하는 함수

	float GetHPRatio();		// 현재 체력 퍼센트 반환
	void DeadEnemy();		// Enemy(= Morigesh)가 죽었을 때 실행될 함수 

	void FireBullet();			// BossBullet 생성함수
	void FireHoming();			// BossHoming 생성함수

	void SpawnLightning();		// Lightning 액터 생성
	void FireLightning();		// Lightning 액터 5개 연속으로 스폰하고, 끝나면 액터제거하는 함수
	void StopLightning();		// Lightning 액터 제거 함수

	void ReadyTornado();		// 토네이도 마크 스폰하는 함수

	void MoveDown();			// Boss 사망 시, 아래로 내려감 + 사이즈 작아짐
	void DeathOracleSpawn();	// Boss 사망 시, 오라클 생성

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
	float detectRange;		// Player 탐지 범위

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void animMontageSetting();			// 애님 몽타주 생성자에서 ConstructorHelpers 함수
	void OnAssetLoadCompleted();		// Enemy 애셋 지정 완료 함수

	UFUNCTION(BlueprintCallable, Category = "bullet")
	void HitBullet(class AGBCharacter* player);

	UFUNCTION(BlueprintCallable, Category = "homing")
	void HitHoming(class AGBCharacter* player);

	// Lightning 액터의 콜리전에 부딪혔을 때, 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "lightning")
	void HitLightning(class AGBCharacter* player);

	// Lightning 액터의 콜리전에 부딪혔을 때, 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "tornado")
	void HitTornado(class AGBCharacter* player);

	FSoftObjectPath enemyAssetToLoad = FSoftObjectPath(nullptr);	// 애셋 경로
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// 애셋 비동기로 로딩

	UPROPERTY(EditDefaultsOnly, Category = GrapplingBox)
	class UBoxComponent* grapBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRange;		// 공격 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRadius;		// 공격 반경 폭

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float maxHP;			// 최대 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float currentHP;		// 현재 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool isDead;			// enemy가 살아있는지 ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FString enemyName;		// enemy 이름

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
	class AGBAIController* EnemyAIController;			// AI 컨트롤러

	FTimerHandle LightningTimerHandle;			// 번개 생성 타이머
	float LightningSpawnInterval;				// 시간 간격

	FTimerHandle LightningDeleteTimerHandle;	// 번개 생성을 멈추는타이머
	float LightningDeleteInterval;				// 시간 간격

	UPROPERTY()
	TArray<class ABossTornado*> arr_Tornado;

	UPROPERTY(VisibleAnywhere, Category = Tornado, Meta = (AllowPrivateAccess = true))
	int32 tornadoCnt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool isInTornado;			// 토네이도에 들어가있는지 ?

	FTimerHandle TornadoTimerHandle;			// 번개 생성 타이머
	FTimerHandle TornadoDeleteTimerHandle;		// 번개 생성을 멈추는타이머
	float TornadoDeleteInterval;				// 시간 간격

	FTimerHandle MoveTimerHandle;				// 보스 몬스터가 죽으면 아래로 내려가게하는 타이머
	float alpha = 0.0f;							// Lerp 함수에 사용할 알파값

	UPROPERTY(EditDefaultsOnly, Category = Death, Meta = (AllowPrivateAccess = true))
	class ABossDeathOracle* DeathOracle;

	// FireBullet 날아오는 소리
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* bulletSFX;

	// 낙뢰 소리
	UPROPERTY(EditDefaultsOnly, Category = Death, Meta = (AllowPrivateAccess = true))
	class USoundBase* lightningSound;

	// Homing 구체 폭발 SFX 효과
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* explosionSFX;

	// 회오리 생성 SFX 효과
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* tornadoStartSFX;

	// 회오리 생성 SFX 효과
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class USoundBase* tornadoSFX;
};
