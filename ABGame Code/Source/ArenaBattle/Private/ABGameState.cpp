// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameState.h"


AABGameState::AABGameState()
{
	totalGameScore = 0;
	bGameCleared = false;	// ���� Ŭ���� ���� false�� �ʱ�ȭ
}

int32 AABGameState::GetTotalGameScore() const
{
	return totalGameScore;	// ���� ���ھ� ��ȯ
}


void AABGameState::AddGameScore()
{
	totalGameScore++;	// NPC�� ��Ƽ� ���� + 1
}

// �̼��� Ŭ������ ������ ���� ����
void AABGameState::SetGameCleared()
{
	bGameCleared = true;
}

// �̼� Ŭ�����ߴ��� ��ȯ
bool AABGameState::IsGameCleared() const
{
	return bGameCleared;
}
