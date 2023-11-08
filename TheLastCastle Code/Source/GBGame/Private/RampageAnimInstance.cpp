// Fill out your copyright notice in the Description page of Project Settings.


#include "RampageAnimInstance.h"


URampageAnimInstance::URampageAnimInstance()
{
	currentSpeed = 0;
}

void URampageAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance�� �������ִ� ���� ������

	if (::IsValid(enemy))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
		currentSpeed = enemy->GetVelocity().Size();
	}
}

void URampageAnimInstance::AnimNotify_AttackHitCheck()
{
	OnHitCheck.Broadcast();
}

void URampageAnimInstance::AnimNotify_ResetCombo()
{
	OnResetCheck.Broadcast();
}

void URampageAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}