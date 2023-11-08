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
			isAir = Character->GetCharacterMovement()->IsFalling();	// �� �ڵ尡 ������ �Ǵ� �Ű���. �𸮾� ������ Ȯ�� !
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	// OnMontageEnded ��������Ʈ�� �ִϸ��̼� ��Ÿ�� ���۰� ���ᰡ �����Ǿ� 
	// if(!Montage_IsPlaying(AttackMontage)) �ڵ� �ʿ� X
	ABCHECK(!isDead);
	Montage_Play(AttackMontage, 1.0f);
}


// NewSection���� ������ �̵��Ѵ�.
void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!isDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}



void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();	// �ִϸ��̼� ��Ÿ�ַ� ���� 'OnAttackHitCheck' ��Ƽ ���̸� �޾�����
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();	// �ִϸ��̼� ��Ÿ�ַ� ���� 'NextAttackCheck' ��Ƽ ���̸� �޾�����
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	// Section ���� 1 ~ 4������ �������� 
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}