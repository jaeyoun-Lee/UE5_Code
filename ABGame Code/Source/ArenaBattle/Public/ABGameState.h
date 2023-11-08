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

	int32 GetTotalGameScore() const;	// ���� ���ھ� ���
	void AddGameScore();				// ���� ���ھ� ���ϱ�

	void SetGameCleared();				// ���� Ŭ���� �ߴ��� ����
	bool IsGameCleared() const;			// ���� Ŭ�����ߴ��� Ȯ��

private:
	UPROPERTY(Transient)
	int32 totalGameScore;	// ���� ���ھ� ������ ����

	UPROPERTY(Transient)
	bool bGameCleared;		// ���� Ŭ�����ߴ����� ���� ���� ����
};
