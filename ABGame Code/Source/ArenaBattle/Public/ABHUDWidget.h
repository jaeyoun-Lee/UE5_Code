// Fillout your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(class UABCharacterStatComponent* CharacterStat);	// 캐릭터 스탯과 바인딩할 함수
	void BindPlayerState(class AABPlayerState* PlayerState);				// 플레이어 스테이트 정보를 바인딩할 함수

protected:
	virtual void NativeConstruct() override;	// UI 시스템이 준비되면 호출되는 함수
	void UpdateCharacterStat();					// 캐릭터 스탯 업데이트
	void UpdatePlayerState();					// 플레이어 스테이트 정보 업데이트

private:
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;	// 캐릭터 스탯 컴포넌트를 저장할 변수
	TWeakObjectPtr<class AABPlayerState> CurrentPlayerState;				// 플레이어 스테이트를 저장할 변수

	UPROPERTY()
	class UProgressBar* HPBar;	// HPBar 프로그레스바 저장할 변수

	UPROPERTY()
	class UProgressBar* ExpBar;	// ExpBar 프로그레스바 저장할 변수

	UPROPERTY()
	class UTextBlock* PlayerName;	// PlayerName 텍스트박스 저장할 변수

	UPROPERTY()
	class UTextBlock* PlayerLevel;	// PlayerLevel 텍스트 박스 저장할 변수

	UPROPERTY()
	class UTextBlock* CurrentScore;	// CurrentScore 텍스트 박스 저장할 변수

	UPROPERTY()
	class UTextBlock* HighScore;	// HighScore 텍스트 박스 저장할 변수

};
