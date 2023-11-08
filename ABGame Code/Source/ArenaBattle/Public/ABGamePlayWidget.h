// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABGamePlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGamePlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClicked();		// 게임으로 돌아가기 버튼 클릭 시,

	UFUNCTION()
	void OnReturnToTitleClicked();	// 타이틀 화면으로 버튼 클릭 시,

	UFUNCTION()
	void OnRetryGameClicked();		// 게임 재시작 버튼 클릭 시,

	UPROPERTY()
	class UButton* ResumeButton;	// 게임으로 돌아가기 버튼

	UPROPERTY()
	class UButton* ReturnToTitleButton;		// 타이틀 화면으로 버튼

	UPROPERTY()
	class UButton* RetryGameButton;		// 게임 재시작 버튼
};
