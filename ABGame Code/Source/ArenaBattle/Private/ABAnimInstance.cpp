// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance() 
{
	currentPawnSpeed = 0;
	isAir = false;
	isDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> attack_Montage(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_Warrior.AM_Warrior'"));

	if (attack_Montage.Succeeded()) {
		AttackMontage = attack_Montage.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!isDead)
	{
		currentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character) {
			isAir = Character->GetCharacterMovement()->IsFalling();	// 이 코드가 문제가 되는 거같음. 언리얼 오류시 확인 !
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	// OnMontageEnded 델리게이트로 애니메이션 몽타주 시작과 종료가 감지되어 
	// if(!Montage_IsPlaying(AttackMontage)) 코드 필요 X
	ABCHECK(!isDead);
	Montage_Play(AttackMontage, 1.0f);
}


// NewSection으로 섹션을 이동한다.
void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!isDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}



void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();	// 애니메이션 몽타주로 부터 'OnAttackHitCheck' 노티 파이를 받았을때
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();	// 애니메이션 몽타주로 부터 'NextAttackCheck' 노티 파이를 받았을때
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	// Section 값이 1 ~ 4사이의 숫자인지 
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}