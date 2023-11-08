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
	UABSaveGame();	// 생성자

	UPROPERTY()
	int32 level;	// 레벨

	UPROPERTY()
	int32 exp;		// 경험치

	UPROPERTY()
	FString playerName;		// 유저 이름

	UPROPERTY()
	int32 highScore;		// 최고 점수

	UPROPERTY()
	int32 CharacterIndex; // 캐릭터 스켈레탈 메시 경로 인덱스
};
