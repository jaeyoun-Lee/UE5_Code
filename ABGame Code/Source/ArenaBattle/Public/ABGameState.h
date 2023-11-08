// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AABGameState();

	int32 GetTotalGameScore() const;	// 현재 스코어 얻기
	void AddGameScore();				// 게임 스코어 더하기

	void SetGameCleared();				// 게임 클리어 했는지 설정
	bool IsGameCleared() const;			// 게임 클리어했는지 확인

private:
	UPROPERTY(Transient)
	int32 totalGameScore;	// 현재 스코어 저장할 변수

	UPROPERTY(Transient)
	bool bGameCleared;		// 게임 클리어했는지에 대한 여부 변수
};
