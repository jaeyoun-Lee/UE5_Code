// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Blueprint/UserWidget.h"
#include "GBEnemyWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPZero);
/**
 * 
 */
UCLASS()
class GBGAME_API UGBEnemyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateHPWidget(float hpRatio);
	void SetName(FString enemyName);

	UPROPERTY()
	FString enemyText;		// enemy 이름

	FOnHPZero OnDie;		// 죽었을 때 호출될 델리게이트

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UProgressBar* hpBar;

	UPROPERTY()
	class UTextBlock* name;	// enemyName 텍스트박스 저장할 변수
};
