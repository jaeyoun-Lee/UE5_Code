// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"
#include "BossTornado.h"

UBossAnimInstance::UBossAnimInstance()
{
	currentSpeed = 0;
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance를 가지고있는 폰을 가져옴

	if (::IsValid(enemy))
	{
		// 캐릭터의 현재 이동속도 전달받음(Idle 상태 vs 걷기 상태)
		currentSpeed = enemy->GetVelocity().Size();
	}
}

void UBossAnimInstance::AnimNotify_AttackHitCheck()
{
	OnHitCheck.Broadcast();
}

void UBossAnimInstance::AnimNotify_ResetCombo()
{
	OnResetCheck.Broadcast();
}

// 사망 애니메이션이 끝나는 지점의 노티파이를 지나면 호출
void UBossAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}


// 유도탄을 발사하는 노티파이를 지나면 호출
void UBossAnimInstance::AnimNotify_AttackBullet()
{
	OnBulletSpawn.Broadcast();
}

// 유도탄을 발사하는 노티파이를 지나면 호출
void UBossAnimInstance::AnimNotify_AttackHoming()
{
	OnHomingSpawn.Broadcast();
}

// Lightning 공격을 하는 노티파이를 지나면 호출
void UBossAnimInstance::AnimNotify_AttackLightning()
{
	OnLightningSpawn.Broadcast();
}

// 토네이도 마크가 생성되기 위한 노티파이를 지나면 호출되는 함수
void UBossAnimInstance::AnimNotify_ReadyTornado()
{
	OnTornadoReady.Broadcast();
}

// 토네이도가 생성되기 위한 노티파이를 지나면 호출되는 함수
void UBossAnimInstance::AnimNotify_AttackTornado()
{
	OnTornadoSpawn.Broadcast();
}