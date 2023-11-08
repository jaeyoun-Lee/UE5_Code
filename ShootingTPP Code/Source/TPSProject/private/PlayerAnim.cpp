// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// �÷��̾��� �̵� �ӵ��� ������ Speed�� �Ҵ��� ����
	// 1. ���� �� ���
	auto ownerPawn = TryGetPawnOwner();

	// 2. ���� ���� �� ĳ���ͷ� �Ҵ�
	auto player = Cast<ATPSPlayer>(ownerPawn);

	// ĳ���� ���� ��,
	if (player)
	{
		// 3. �̵� �ӵ� �ʿ�
		FVector velocity = player->GetVelocity();

		// 4. ĳ������ ���� ���� ã��
		FVector forward = player->GetActorForwardVector();

		// 5. �̵� �ӵ� ���Ϳ� ĳ���� ���� ���� ����
		//	speed = velocity.Dot(forward); �Ʒ��� ���� �ڵ�
		speed = FVector::DotProduct(velocity, forward);

		// 6. �¿� �ӵ��Ҵ��ϱ�
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(velocity, rightVector);

	// �÷��̾ ���� ���߿� �ִ��� ���� ���
	// IsFalling() �Լ��� �÷��̾ ���߿� ������ true, �ƴϸ� false�� ��ȯ
	// �� IsFalling() �Լ��� ����ϱ� ���ؼ��� GetCharacterMovement�Լ��� UCharacterMovementComponent Ŭ������ �ν��Ͻ��� �����;��Ѵ�.
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
	UE_LOG(LogTemp, Warning, TEXT("Fire Montage ���"));
}