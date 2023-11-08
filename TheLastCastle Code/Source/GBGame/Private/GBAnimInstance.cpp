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


// Tick() 함수와 같은 역할
void UGBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	AGBCharacter* GBPlayer = nullptr;

	auto player = TryGetPawnOwner();
	if (::IsValid(player))
	{
		// 캐릭터의 현재 이동속도 전달받음(Idle 상태 vs 걷기 상태)
		currentSpeed = player->GetVelocity().Size();

		auto Character = Cast<ACharacter>(player);
		if (Character)
		{
			isAir = Character->GetMovementComponent()->IsFalling();

			GBPlayer = Cast<AGBCharacter>(Character);
			if (GBPlayer)
			{
				// 점프키를 눌러서 점프한 것이면, 회전 점프하도록
				isJumping = GBPlayer->isJumping;	// 만약 점프키를 누르지 않고 공중에 있는 것이면 낙하하는 것이므로 점프 없이 JumpLoop가 바로 실행되도록
			}
		}
	}

	if (!isAir)
	{
		bAirAttack = false;	// 땅에 도착해야 bAirAttack을 false로 변경
		
		if (GBPlayer != nullptr)
		{
			GBPlayer->isJumping = false;	// 땅에 도착 시, 초기화
		}
	}
}

// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
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




