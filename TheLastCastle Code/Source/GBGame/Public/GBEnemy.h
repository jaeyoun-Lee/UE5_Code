// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Character.h"
#include "GBEnemy.generated.h"


UCLASS()
class GBGAME_API AGBEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGBEnemy();

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

	void DeadEnemy();		// Enemy가 죽으면 실행되는 함수
	virtual void animMontageSetting();
	
	UPROPERTY(EditDefaultsOnly, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	class UGBEnemyWidget* enemyWidget;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnAssetLoadCompleted();		// Enemy 애셋 지정 완료 함수

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* hitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* attackMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* deathMontage;

	FSoftObjectPath enemyAssetToLoad = FSoftObjectPath(nullptr);	// 애셋 경로
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// 애셋 비동기로 로딩

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
	class UGBEnemyAnimInstance* EnemyAnim;

	UPROPERTY()
	class AGBAIController* EnemyAIController;			// AI 컨트롤러
};
