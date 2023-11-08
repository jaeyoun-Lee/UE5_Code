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
	void OnResumeClicked();		// �������� ���ư��� ��ư Ŭ�� ��,

	UFUNCTION()
	void OnReturnToTitleClicked();	// Ÿ��Ʋ ȭ������ ��ư Ŭ�� ��,

	UFUNCTION()
	void OnRetryGameClicked();		// ���� ����� ��ư Ŭ�� ��,

	UPROPERTY()
	class UButton* ResumeButton;	// �������� ���ư��� ��ư

	UPROPERTY()
	class UButton* ReturnToTitleButton;		// Ÿ��Ʋ ȭ������ ��ư

	UPROPERTY()
	class UButton* RetryGameButton;		// ���� ����� ��ư
};
