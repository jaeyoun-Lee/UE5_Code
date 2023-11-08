// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "GBEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttckHitCheckDelegate);		// ������ �����ߴ��� Ȯ���ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);			// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnEnemyDieDelegate);			// Enemy�� ������ ȣ��Ǵ� ��������Ʈ

/**
 * 
 */
UCLASS()
class GBGAME_API UGBEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGBEnemyAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnResetCheckDelegate OnResetCheck;			// ������ �����ߴ��� Ȯ���ϴ� ��������Ʈ ����
	FOnAttckHitCheckDelegate OnHitCheck;		// ������ �ʱ�ȭ�ϴ� ��������Ʈ ����
	FOnEnemyDieDelegate OnEnemyDie;				// Enemy�� ������ ȣ��Ǵ� ��������Ʈ ����
	
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
