// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/PlayerController.h"
#include "GBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API AGBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGBPlayerController();			// 생성자

	virtual void PostInitializeComponents() override;	// 폰과 플레이어 컨트롤러가 생성되는 시점
	virtual void OnPossess(APawn* aPawn) override;		// 빙의하는 시점

	class UGBHUDWidget* GetHUDWidget() const;			// HUDWidget 가져오기

	void ChangeInputMode(bool bGameMode = true);		// 입력 모드 변경(게임 입력 <> UI 입력)

	UFUNCTION(BlueprintCallable)

	void ShowSucceededUI();								// 게임 클리어 시, Clear UI 띄우기

	void ShowFailedUI();								// 플레이어 사망 시, Failed UI 띄우기

	UFUNCTION(BlueprintCallable)
	void ShowNextUI();									// 다음 레벨로 넘어갈 때, NextWidget UI 띄우기

	UFUNCTION(BlueprintCallable)
	class UNextLevelWidget* GetNextLevelWidget();		// 다음 레벨로 넘어가는 위젯 Get하기

	UFUNCTION(BlueprintCallable)						// Boss 생성 함수
	void BossSpawn();


protected:
	virtual void BeginPlay() override;

	// 플레이어의 입력을 받아드리는 함수
	virtual void SetupInputComponent() override;

	// HUD위젯 클래스와 이를 상속받은 클래스들을 가져오기 위한 선언(특정 클래스와 상속받은 클래스들로 목록을 한정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBHUDWidget> HUDWidgetClass;			

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> ClearWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> PauseWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> FailedWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UNextLevelWidget> NextLevelWidgetClass;

private:
	void OnPause();			// 게임 일시정지 함수

	UPROPERTY()
	class UGBHUDWidget* HUDWidget;		// HUD 위젯 변수

	UPROPERTY()
	class UGBStartWidget* clearWidget;

	UPROPERTY()
	class UGBStartWidget* pauseWidget;

	UPROPERTY()
	class UGBStartWidget* failedWidget;

	UPROPERTY()
	class UNextLevelWidget* nextWidget;

	FInputModeGameOnly gameInputMode;
	FInputModeUIOnly uiInputMode;

	UPROPERTY(VisibleAnywhere, Category = Boss, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ABoss> bossSpawnClass;
};
