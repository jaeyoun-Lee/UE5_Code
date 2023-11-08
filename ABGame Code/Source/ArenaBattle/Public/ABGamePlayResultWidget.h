// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABGamePlayWidget.h"
#include "ABGamePlayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGamePlayResultWidget : public UABGamePlayWidget
{
	GENERATED_BODY()

public:
	// 미리 UI 위젯이 게임 스테이트 정보를 읽어들일 수 있도록 바인딩할 함수
	void BindGameState(class AABGameState* gameState);

protected:
	virtual void NativeConstruct() override;

private:
	// 현재 게임 스테이트 변수
	TWeakObjectPtr<class AABGameState> currentGameState;
};
