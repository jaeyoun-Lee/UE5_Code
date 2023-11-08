// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameState.h"


AABGameState::AABGameState()
{
	totalGameScore = 0;
	bGameCleared = false;	// 게임 클리어 여부 false로 초기화
}

int32 AABGameState::GetTotalGameScore() const
{
	return totalGameScore;	// 현재 스코어 반환
}


void AABGameState::AddGameScore()
{
	totalGameScore++;	// NPC를 잡아서 점수 + 1
}

// 미션을 클리어한 것으로 설정 변경
void AABGameState::SetGameCleared()
{
	bGameCleared = true;
}

// 미션 클리어했는지 반환
bool AABGameState::IsGameCleared() const
{
	return bGameCleared;
}
