// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSaveGame.h"


// 생성자에서 기본 데이터 값 초기화
UABSaveGame::UABSaveGame()
{
	level = 1;
	exp = 0;
	playerName = TEXT("Guest");
	highScore = 0;
	CharacterIndex = 0;
}