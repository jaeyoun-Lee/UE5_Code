// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Blueprint/UserWidget.h"
#include "NextLevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UNextLevelWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UWidgetAnimation* GetWidgetAnimation();
	UWidgetAnimation* GetShowNameAnimation();
	UWidgetAnimation* GetShowUEAnimation();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* NextLevel;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ShowName;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ShowUE;
};
