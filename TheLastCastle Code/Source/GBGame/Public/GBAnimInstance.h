// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "GBAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);		// 공격을 초기화하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnShockCheckDelegate);		// 전기 공격이 끝났음을 알리는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeadDelegate);			// 플레이어 사망을 알리는 델리게이트

/**
 * 
 */
UCLASS()
class GBGAME_API UGBAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UGBAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	// Tick() 함수와 같은 역할

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnResetCheckDelegate OnResetCheck;
	FOnShockCheckDelegate OnShockCheck;
	FOnPlayerDeadDelegate OnPlayerDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isAir;		// 공중에 있는지 ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable, Meta = (AllowPrivateAccess = true))
	bool bAirAttack;		// 공중 공격을 했는지 ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isJumping;			// 스페이스바로 점프한 것인지 ?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable)
	bool isGrappling;		// 갈고리를 걸고 날라가는중인지 ?

private:

	// 공격 몽타주의 AttackHitCheck 노티파이가 호출되면,
	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackHitCheck(); 

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_bNextAttack();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_ResetCombo();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_EndShock();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_GameEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Variable, Meta = (AllowPrivateAccess = true))
	float currentSpeed;		// 현재 속도
};
