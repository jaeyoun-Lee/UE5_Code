// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UABSaveGame();	// ������

	UPROPERTY()
	int32 level;	// ����

	UPROPERTY()
	int32 exp;		// ����ġ

	UPROPERTY()
	FString playerName;		// ���� �̸�

	UPROPERTY()
	int32 highScore;		// �ְ� ����

	UPROPERTY()
	int32 CharacterIndex; // ĳ���� ���̷�Ż �޽� ��� �ε���
};
