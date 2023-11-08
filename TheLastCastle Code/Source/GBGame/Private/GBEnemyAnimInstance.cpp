// Fill out your copyright notice in the Description page of Project Settings.


#include "GBEnemyAnimInstance.h"


UGBEnemyAnimInstance::UGBEnemyAnimInstance()
{
	currentSpeed = 0;
}

void UGBEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance를 가지고있는 폰을 가져옴
	
	if (::IsValid(enemy))
	{
		// 캐릭터의 현재 이동속도 전달받음(Idle 상태 vs 걷기 상태)
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