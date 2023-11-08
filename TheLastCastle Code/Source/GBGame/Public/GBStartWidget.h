// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Blueprint/UserWidget.h"
#include "GBStartWidget.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UGBStartWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnLobbyClicked();

	UFUNCTION()
	void OnExitClicked();


	UPROPERTY()
	class UButton* StartButton;

	UPROPERTY()
	class UButton* ContinueButton;

	UPROPERTY()
	class UButton* RestartButton;

	UPROPERTY()
	class UButton* LobbyButton;

	UPROPERTY()
	class UButton* QuitButton;
};
