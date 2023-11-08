// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Blueprint/UserWidget.h"
#include "GBHUDWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerDie);
DECLARE_MULTICAST_DELEGATE(FOnBossDie);

/**
 * 
 */
UCLASS()
class GBGAME_API UGBHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdatePlayerHP(float hpRatio);		// 체력이 변경되면 실행할 함수(프로그레스바와 텍스트 블록 수정)
	void UpdateEnemyCnt(int32 eCnt);		// 체력이 변경되면 실행할 함수(프로그레스바와 텍스트 블록 수정)

	void UpdateBossHP(float hpRatio);		// 보스의 체력이 변경되면 실행할 함수(프로그레스바와 텍스트 블록 수정)

	FOnPlayerDie OnPlayerDie;				// 플레이어 체력이 0이하가되면 호출될 델리게이트
	FOnBossDie OnDie;

protected:
	// UI 초기화 되는 시점에 호출되는 함수
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UProgressBar* hpBar;			// 체력 ProgreesBar

	UPROPERTY()
	class UTextBlock* hpValue;			// 체력 퍼센트를 보여줄 텍스트 블록

	UPROPERTY()
	class UTextBlock* enemyCnt;			// 남은 Enemy 수를 보여줄 텍스트 블록

	UPROPERTY()
	class UProgressBar* bossHP;			// 보스 체력 ProgreesBar

	UPROPERTY()
	class UTextBlock* remainEnemy;		// Remaining Enemy 텍스트 블록 

	UPROPERTY()
	class UImage* bossImage;		// Remaining Enemy 텍스트 블록 
};
