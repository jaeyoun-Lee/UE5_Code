// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "GBAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);		// ������ �ʱ�ȭ�ϴ� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnShockCheckDelegate);		// ���� ������ �������� �˸��� ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeadDelegate);			// �÷��̾� ����� �˸��� ��������Ʈ

/**
 * 
 */
UCLASS()
class GBGAME_API UGBAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UGBAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// Tick() �Լ��� ���� ����

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnResetCheckDelegate OnResetCheck;
	FOnShockCheckDelegate OnShockCheck;
	FOnPlayerDeadDelegate OnPlayerDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isAir;		// ���߿� �ִ��� ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable, Meta = (AllowPrivateAccess = true))
	bool bAirAttack;		// ���� ������ �ߴ��� ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isJumping;			// �����̽��ٷ� ������ ������ ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isGrappling;		// ������ �ɰ� ���󰡴������� ?

private:

	// ���� ��Ÿ���� AttackHitCheck ��Ƽ���̰� ȣ��Ǹ�,
	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackHitCheck(); 

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_bNextAttack();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_ResetCombo();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_EndShock();

	// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
	UFUNCTION()
	void AnimNotify_GameEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable, Meta = (AllowPrivateAccess = true))
	float currentSpeed;		// ���� �ӵ�
};
