// Fill out your copyright notice in the Description page of Project Settings.


#include "KhaimeraAnimInstance.h"


UKhaimeraAnimInstance::UKhaimeraAnimInstance()
{
	currentSpeed = 0;
}


void UKhaimeraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance�� �������ִ� ���� ������

	if (::IsValid(enemy))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
		currentSpeed = enemy->GetVelocity().Size();
	}
}

void UKhaimeraAnimInstance::AnimNotify_AttackHitCheck()
{
	OnHitCheck.Broadcast();
}

void UKhaimeraAnimInstance::AnimNotify_ResetCombo()
{
	OnResetCheck.Broadcast();
}

void UKhaimeraAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}