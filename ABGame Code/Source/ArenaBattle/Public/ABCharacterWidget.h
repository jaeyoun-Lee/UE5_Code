// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABCharacterStatComponent.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 캐릭터 스탯과 UI를 연결하는 함수
	void BindCharacterStat(UABCharacterStatComponent* NewCharacterStat);

protected:
	// UI 위젯을 초기화하는 시점에서 발생하는 함수
	virtual void NativeConstruct() override;

	// 프로그레스 바의 퍼센트를 변경하는 함수
	void UpdateHPWidget();

private:
	// 캐릭터 스탯 컴포넌트를 참조
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;

	UPROPERTY()
	class UProgressBar* HP_Progressbar;	// 프로그레스바
};
