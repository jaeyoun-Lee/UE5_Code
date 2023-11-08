// Fill out your copyright notice in the Description page of Project Settings.


#include "MorigeshAnimInstance.h"


UMorigeshAnimInstance::UMorigeshAnimInstance()
{
	currentSpeed = 0;
}

void UMorigeshAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance�� �������ִ� ���� ������

	if (::IsValid(enemy))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
		currentSpeed = enemy->GetVelocity().Size();
	}
}

void UMorigeshAnimInstance::AnimNotify_AttackHitCheck()
{
	OnHitCheck.Broadcast();
}

void UMorigeshAnimInstance::AnimNotify_ResetCombo()
{
	OnResetCheck.Broadcast();
}

void UMorigeshAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}