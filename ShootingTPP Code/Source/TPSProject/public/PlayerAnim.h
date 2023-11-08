// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 플레이어 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float speed = 0;

	// 플레이어 좌우 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float direction = 0;

	// 매 프레임 갱신되는 함수
	// 블루 프린트에 있던 Event Blueprint Update Animation 노드에 해당하는 함수이다.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 플레이어가 공중에 있는지 확인하는 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;


	// 재생할 공격 애니메이션 몽타주(애니메이션 재생을 위한 처리)
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	
	class UAnimMontage* attackAnimMontage;

	// 공격 애니메이션 재생 함수
	void PlayAttackAnim();
};
