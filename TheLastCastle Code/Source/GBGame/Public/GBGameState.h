// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/GameStateBase.h"
#include "GBGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameClearDelegate);
/**
 * 
 */
UCLASS()
class GBGAME_API AGBGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGBGameState();				// 생성자

	int32 RemainingEnemy();		// 게임 시작 시, Enemy가 몇명이나 있는지 카운트하는 함수
	int32 GetEnemyCount();		// 현재 남아있는 Enemy 수 반환

	UFUNCTION(BlueprintCallable)
	void killedEnemy();			// Enemy가 죽으면 실행되는 함수

	FOnGameClearDelegate OnGameClear;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy, Meta = (AllowPrivateAccess = true))
	int32 enemyCount;			// 현재 Enemy 수

	UPROPERTY(EditAnywhere, Category = ProtalLocation, Meta = (AllowPrivateAccess = true));
	FVector stage1_Location;

	UPROPERTY(EditAnywhere, Category = ProtalLocation, Meta = (AllowPrivateAccess = true));
	FVector stage2_Location;
};
