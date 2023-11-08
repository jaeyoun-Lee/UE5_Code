// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLevelUPDELEGATE);
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();	// 생성자

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;	// 빙의하는 시점

	class UABHUDWidget* GetHudWidget() const;	// HUD위젯 가져오는 함수

	void NPCKill(class AABCharacter* KilledNPC) const;	// NPC를 죽이면 방금 잡은 NPC의 경험치를 내 경험치로 획득

	void AddGameScore() const;		// 게임 스코어를 추가하는 

	void ChangeInputMode(bool bGameMode = true);	// 게임에 입력되는 모드를 설정하는 함수(ex : UI에만 입력 전달 / 게임에만 입력 전달 등...)

	void ShowResultUI();	// 결과 UI를 보여주는 함수

	FOnLevelUPDELEGATE OnLevelUP;	// 레벨업 시 호출될 델리게이트(체력을 풀로 채움)

protected:
	virtual void BeginPlay() override;

	// HUD위젯 클래스와 이를 상속받은 클래스들을 가져오기 위한 선언(특정 클래스와 상속받은 클래스들로 목록을 한정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;	

	// 입력을 처리하기 위한 함수
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamePlayWidget> MenuWidgetClass;	// 게임 일시정지 UI 위젯들 담을 변수

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamePlayResultWidget> ResultWidgetClass;	// 게임 결과 UI 위젯을 담을 변수

private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget;	// HUD 위젯 변수

	UPROPERTY()	
	class AABPlayerState* ABPlayerState;	// PlayerState 변수

	UPROPERTY()
	class UABGamePlayWidget* MenuWidget;	// 게임 일시정지 UI 변수

	UPROPERTY()
	class UABGamePlayResultWidget* resultWidget;	// 게임 결과 UI 변수

	FInputModeGameOnly gameInputMode;			// 게임에만 입력을 전달하도록하는 변수
	FInputModeUIOnly UIInputMode;				// UI에만 입력을 전달하도록하는 변수

	// P키가 눌리면 호출될 함수
	void OnGamePause();

// Pawn으로 가는 입력을 막는 코드
//protected:
//	virtual void SetupInputComponent() override;
//
//private:
//	void LeftRight(float NewAxisValue);
};
