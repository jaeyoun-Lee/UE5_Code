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

	auto enemy = TryGetPawnOwner();		// GBEnemyAnimInstance�� �������ִ� ���� ������

	if (::IsValid(enemy))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
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

// ��� �ִϸ��̼��� ������ ������ ��Ƽ���̸� ������ ȣ��
void UBossAnimInstance::AnimNotify_OnDead()
{
	OnEnemyDie.Broadcast();
}


// ����ź�� �߻��ϴ� ��Ƽ���̸� ������ ȣ��
void UBossAnimInstance::AnimNotify_AttackBullet()
{
	OnBulletSpawn.Broadcast();
}

// ����ź�� �߻��ϴ� ��Ƽ���̸� ������ ȣ��
void UBossAnimInstance::AnimNotify_AttackHoming()
{
	OnHomingSpawn.Broadcast();
}

// Lightning ������ �ϴ� ��Ƽ���̸� ������ ȣ��
void UBossAnimInstance::AnimNotify_AttackLightning()
{
	OnLightningSpawn.Broadcast();
}

// ����̵� ��ũ�� �����Ǳ� ���� ��Ƽ���̸� ������ ȣ��Ǵ� �Լ�
void UBossAnimInstance::AnimNotify_ReadyTornado()
{
	OnTornadoReady.Broadcast();
}

// ����̵��� �����Ǳ� ���� ��Ƽ���̸� ������ ȣ��Ǵ� �Լ�
void UBossAnimInstance::AnimNotify_AttackTornado()
{
	OnTornadoSpawn.Broadcast();
}