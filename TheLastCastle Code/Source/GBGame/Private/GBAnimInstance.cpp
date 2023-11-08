// Fill out your copyright notice in the Description page of Project Settings.


#include "GBAnimInstance.h"
#include "GBPlayerController.h"
#include "GBCharacter.h"


UGBAnimInstance::UGBAnimInstance()
{
	currentSpeed = 0.0f;
	isAir = false;
	bAirAttack = false;
	isGrappling = false;
}


// Tick() �Լ��� ���� ����
void UGBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	AGBCharacter* GBPlayer = nullptr;

	auto player = TryGetPawnOwner();
	if (::IsValid(player))
	{
		// ĳ������ ���� �̵��ӵ� ���޹���(Idle ���� vs �ȱ� ����)
		currentSpeed = player->GetVelocity().Size();

		auto Character = Cast<ACharacter>(player);
		if (Character)
		{
			isAir = Character->GetMovementComponent()->IsFalling();

			GBPlayer = Cast<AGBCharacter>(Character);
			if (GBPlayer)
			{
				// ����Ű�� ������ ������ ���̸�, ȸ�� �����ϵ���
				isJumping = GBPlayer->isJumping;	// ���� ����Ű�� ������ �ʰ� ���߿� �ִ� ���̸� �����ϴ� ���̹Ƿ� ���� ���� JumpLoop�� �ٷ� ����ǵ���
			}
		}
	}

	if (!isAir)
	{
		bAirAttack = false;	// ���� �����ؾ� bAirAttack�� false�� ����
		
		if (GBPlayer != nullptr)
		{
			GBPlayer->isJumping = false;	// ���� ���� ��, �ʱ�ȭ
		}
	}
}

// �𸮾� ������ �ڵ����� �ִ� �ν��Ͻ� Ŭ������ AnimNotify_��Ƽ���̸� �̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ���Ѵ�.
void UGBAnimInstance::AnimNotify_AttackHitCheck()
{
	GBLOG(Warning, TEXT(" Called by AttackHitCheck Notify"));
	OnAttackHitCheck.Broadcast();
}

void UGBAnimInstance::AnimNotify_bNextAttack()
{
	OnNextAttackCheck.Broadcast();
}

void UGBAnimInstance::AnimNotify_ResetCombo()
{
	GBLOG(Warning, TEXT("Combo is Reset"));
	OnResetCheck.Broadcast();
}

void UGBAnimInstance::AnimNotify_EndShock()
{
	GBLOG(Warning, TEXT("Lightning is End"));
	OnShockCheck.Broadcast();
}

void UGBAnimInstance::AnimNotify_GameEnd()
{
	OnPlayerDead.Broadcast();
}




