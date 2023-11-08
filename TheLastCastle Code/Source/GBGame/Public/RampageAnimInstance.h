// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "RampageAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttckHitCheckDelegate);		// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);		// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnEnemyDieDelegate);		// ������ �ʱ�ȭ�ϴ� ��������Ʈ
/**
 * 
 */
UCLASS()
class GBGAME_API URampageAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	URampageAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnResetCheckDelegate OnResetCheck;
	FOnAttckHitCheckDelegate OnHitCheck;
	FOnEnemyDieDelegate OnEnemyDie;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Speed, Meta = (AllowPrivateAccess = true));
	float currentSpeed;

};
