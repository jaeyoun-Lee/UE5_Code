// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// 노티파이에 대한 델리게이트 생성
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 몽타주(= 공격 애니메이션 모음) 재생 함수
	void PlayAttackMontage();

	// NewSection으로 섹션을 이동한다.
	void JumpToAttackMontageSection(int32 NewSection);

	// 사망 상태로 변환해주는 함수
	void SetDeadAnim() { isDead = true; }

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;	// 다음 공격을 이어할지 확인하는 노티파이
	FOnAttackHitCheckDelegate OnAttackHitCheck;		// 공격을 시작함을 알리는 노티파이

private:
	// 애님 그래프에서 사용할 수 있도록 EditAnywhere 사용
	// 추가로, 블루 프린트에서 호출할 수 있도록 BlueprintReadOnly 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = status, Meta = (AllowPrivateAccess = true))
	float currentPawnSpeed;

	// 현재 공중에 있는지 확인하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, Meta = (AllowPrivateAccess = true))
	bool isAir;

	// 캐릭터가 대미지를 받아 죽었는지 확인하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, Meta = (AllowPrivateAccess = true))
	bool isDead;


	// 만든 애니메이션 몽타주 애셋을 담을 변수
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	// 애니메이션 몽타주의 AttackHitCheck 노티파이
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	// 애니메이션 몽타주의 NextAttackCheck 노티파이
	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	// 몽타주 섹션의 이름을 가져오는 함수
	FName GetAttackMontageSectionName(int32 Section);
};
