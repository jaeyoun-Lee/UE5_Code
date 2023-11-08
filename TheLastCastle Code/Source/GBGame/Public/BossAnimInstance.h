// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "BossAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttckHitCheckDelegate);		// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);			// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnEnemyDieDelegate);			// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnBulletSpawnDelegate);			// ȭ�� ����ü�� �����ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnHomingSpawnDelegate);			// ����ź ����ü�� �����ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnLightningSpawnDelegate);		// Lightning ���� �����ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnTornadoReadyDelegate);		// ����̵� ���� �غ��ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnTornadoSpawnDelegate);		// ����̵� ���� �����ϴ� ��������Ʈ

/**
 * 
 */
UCLASS()
class GBGAME_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UBossAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnResetCheckDelegate OnResetCheck;
	FOnAttckHitCheckDelegate OnHitCheck;
	FOnEnemyDieDelegate OnEnemyDie;

	FOnBulletSpawnDelegate OnBulletSpawn;
	FOnHomingSpawnDelegate OnHomingSpawn;
	FOnLightningSpawnDelegate OnLightningSpawn;
	FOnTornadoReadyDelegate OnTornadoReady;
	FOnTornadoSpawnDelegate OnTornadoSpawn;

private:
	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackHitCheck();


	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_ResetCombo();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_OnDead();


	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackBullet();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackHoming();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackLightning();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_ReadyTornado();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackTornado();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Speed, Meta = (AllowPrivateAccess = true));
	float currentSpeed;
};
