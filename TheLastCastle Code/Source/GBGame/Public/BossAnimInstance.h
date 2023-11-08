// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Animation/AnimInstance.h"
#include "BossAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttckHitCheckDelegate);		// 공격을 초기화하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnResetCheckDelegate);			// 공격을 초기화하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnEnemyDieDelegate);			// 공격을 초기화하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnBulletSpawnDelegate);			// 화염 투사체를 생성하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnHomingSpawnDelegate);			// 유도탄 투사체를 생성하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnLightningSpawnDelegate);		// Lightning 공격 생성하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnTornadoReadyDelegate);		// 토네이도 공격 준비하는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnTornadoSpawnDelegate);		// 토네이도 공격 생성하는 델리게이트

/**
 * 
 */
UCLASS()
class GBGAME_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UBossAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnResetCheckDelegate OnResetCheck;
	FOnAttckHitCheckDelegate OnHitCheck;
	FOnEnemyDieDelegate OnEnemyDie;

	FOnBulletSpawnDelegate OnBulletSpawn;
	FOnHomingSpawnDelegate OnHomingSpawn;
	FOnLightningSpawnDelegate OnLightningSpawn;
	FOnTornadoReadyDelegate OnTornadoReady;
	FOnTornadoSpawnDelegate OnTornadoSpawn;

private:
	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackHitCheck();


	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_ResetCombo();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_OnDead();


	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackBullet();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackHoming();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackLightning();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_ReadyTornado();

	// 언리얼 엔진은 자동으로 애님 인스턴스 클래스의 AnimNotify_노티파이명 이라는 이름의 멤버 함수를 찾아서 호출한다.
	UFUNCTION()
	void AnimNotify_AttackTornado();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Speed, Meta = (AllowPrivateAccess = true));
	float currentSpeed;
};
