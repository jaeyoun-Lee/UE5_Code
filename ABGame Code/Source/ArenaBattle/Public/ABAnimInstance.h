// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// ��Ƽ���̿� ���� ��������Ʈ ����
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// ��Ÿ��(= ���� �ִϸ��̼� ����) ��� �Լ�
	void PlayAttackMontage();

	// NewSection���� ������ �̵��Ѵ�.
	void JumpToAttackMontageSection(int32 NewSection);

	// ��� ���·� ��ȯ���ִ� �Լ�
	void SetDeadAnim() { isDead = true; }

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;	// ���� ������ �̾����� Ȯ���ϴ� ��Ƽ����
	FOnAttackHitCheckDelegate OnAttackHitCheck;		// ������ �������� �˸��� ��Ƽ����

private:
	// �ִ� �׷������� ����� �� �ֵ��� EditAnywhere ���
	// �߰���, ��� ����Ʈ���� ȣ���� �� �ֵ��� BlueprintReadOnly �߰�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = status, Meta = (AllowPrivateAccess = true))
	float currentPawnSpeed;

	// ���� ���߿� �ִ��� Ȯ���ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, Meta = (AllowPrivateAccess = true))
	bool isAir;

	// ĳ���Ͱ� ������� �޾� �׾����� Ȯ���ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, Meta = (AllowPrivateAccess = true))
	bool isDead;


	// ���� �ִϸ��̼� ��Ÿ�� �ּ��� ���� ����
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	// �ִϸ��̼� ��Ÿ���� AttackHitCheck ��Ƽ����
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	// �ִϸ��̼� ��Ÿ���� NextAttackCheck ��Ƽ����
	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	// ��Ÿ�� ������ �̸��� �������� �Լ�
	FName GetAttackMontageSectionName(int32 Section);
};
