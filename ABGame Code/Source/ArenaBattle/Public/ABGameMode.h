// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AABGameMode();

	virtual void PostInitializeComponents() override;		// 폰과 플레이어 컨트롤러가 생성되는 시점
	virtual void PostLogin(APlayerController* NewPlayer) override;		// 플레이어 컨트롤러 구성을 완료하는 시점
	void AddScore(class AABPlayerController* ScoredPlayer);	// 전달받은 플레이어 컨트롤러의 점수 증가

	int32 GetScore() const;			// 현재 게임 스코어 가져오기

private:
	UPROPERTY()
	class AABGameState* ABGameState;	// 게임 스테이트를 저장할 변수

	UPROPERTY()
	int32 ScoreToClear;				// 미션 성공 여부를 위해 획득해야하는 점수 컷
};
