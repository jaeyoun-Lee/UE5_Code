// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 플레이어의 이동 속도를 가져와 Speed에 할당할 예정
	// 1. 소유 폰 얻기
	auto ownerPawn = TryGetPawnOwner();

	// 2. 얻은 소유 폰 캐릭터로 할당
	auto player = Cast<ATPSPlayer>(ownerPawn);

	// 캐스팅 성공 시,
	if (player)
	{
		// 3. 이동 속도 필요
		FVector velocity = player->GetVelocity();

		// 4. 캐릭터의 전방 벡터 찾기
		FVector forward = player->GetActorForwardVector();

		// 5. 이동 속도 벡터와 캐릭터 전방 벡터 내적
		//	speed = velocity.Dot(forward); 아래와 같은 코드
		speed = FVector::DotProduct(velocity, forward);

		// 6. 좌우 속도할당하기
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(velocity, rightVector);

	// 플레이어가 현재 공중에 있는지 여부 기억
	// IsFalling() 함수는 플레이어가 공중에 있으면 true, 아니면 false를 반환
	// 이 IsFalling() 함수를 사용하기 위해서는 GetCharacterMovement함수로 UCharacterMovementComponent 클래스의 인스턴스를 가져와야한다.
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
	UE_LOG(LogTemp, Warning, TEXT("Fire Montage 재생"));
}