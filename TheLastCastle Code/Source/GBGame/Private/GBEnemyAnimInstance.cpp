// Fill out your copyright notice in the Description page of Project Settings.


#include "GBEnemyAnimInstance.h"


UGBEnemyAnimInstance::UGBEnemyAnimInstance()
{
	currentSpeed = 0;
}

void UGBEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance�� �������ִ� ���� ������
	
	if (::IsValid(enemy))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
		currentSpeed = enemy->GetVelocity().Size();
	}
}

void UGBEnemyAnimInstance::AnimNotify_AttackHitCheck()
{
	OnHitCheck.Broadcast();
}

void UGBEnemyAnimInstance::AnimNotify_ResetCombo()
{
	OnResetCheck.Broadcast();
}

void UGBEnemyAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}