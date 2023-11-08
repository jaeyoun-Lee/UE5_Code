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

	// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����


	// player�� �߰��Ͽ� ȸ���� �� ���ڿ��������� �ذ��ϱ� ���� ���
	// ���� ���� �Ǿ��� �� ����(�� �Լ��� READY ������Ʈ���� ����)
	virtual void PossessedBy(AController* newController) override;

	void Attack();			// ���� 
	void AttackCheck();		// ������ ��ȿ���� Ȯ���ϴ� �Լ�

	float GetHPRatio();		// ���� ü�� �ۼ�Ʈ ��ȯ

	void DeadEnemy();		// Enemy�� ������ ����Ǵ� �Լ�
	virtual void animMontageSetting();
	
	UPROPERTY(EditDefaultsOnly, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	class UGBEnemyWidget* enemyWidget;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnAssetLoadCompleted();		// Enemy �ּ� ���� �Ϸ� �Լ�

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* hitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* attackMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* deathMontage;

	FSoftObjectPath enemyAssetToLoad = FSoftObjectPath(nullptr);	// �ּ� ���
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// �ּ� �񵿱�� �ε�

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
	class UGBEnemyAnimInstance* EnemyAnim;

	UPROPERTY()
	class AGBAIController* EnemyAIController;			// AI ��Ʈ�ѷ�
};
