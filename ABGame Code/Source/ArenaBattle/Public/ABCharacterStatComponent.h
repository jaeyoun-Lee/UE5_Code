// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);	// 체력이 0이하일 때 동작하는 델리게이트 생성
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);	// HP값이 변경되면 호출되는 델리게이트 생성

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 액터의 PostInitializeComponent() 함수에 대응하기 위해 사용하는 함수
	virtual void InitializeComponent() override;

	void SetNewLevel(int32 newLevel);	// 레벨 설정

	void SetDamage(float NewDamage);	// 대미지를 받는 함수
	float GetAttack();					// 피해 입힐 대미지 전달

	void SetHP(float NewHP);			// 변경된 HP 설정 함수
	float GetHPRatio();					// 남은 HP의 비율을 반환하는 함수(= 프로그레스바 비율)

	int32 GetDropExp() const;		// NPC가 드롭하는 경험치 양 얻기

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangeDelegate OnHPChanged;

private:
	struct FABCharacterData* currentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};
