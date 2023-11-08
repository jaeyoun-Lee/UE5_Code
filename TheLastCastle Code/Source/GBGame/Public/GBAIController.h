// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "AIController.h"
#include "GBAIController.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API AGBAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGBAIController();		// 생성자

	virtual void OnPossess(APawn* InPawn) override;		// 빙의하는 시점
	virtual void OnUnPossess() override;				// 빙의 해제 함수

	static const FName HomePosKey;						// Enemy 시작 벡터	
	static const FName PatrolPosKey;					// 순찰할 위치 벡터
	static const FName TargetKey;						// Target(= Player) 오브젝트를 저장할 Object타입

	void RunAI();		// AI가 동작할 수 있게 하는 함수
	void StopAI();		// AI 동작을 정지시키는 함수

private:
	void OnRepeatTimer();				// 타이머를 설치하여 지정한 시간마다 폰에게 목적지로 이동하는 명령

	FTimerHandle RepeatTimerHandle;		// 타이머
	float repeatInterval;				// 시간 간격

	UPROPERTY()	
	class UBehaviorTree* BTAsset;		// Enemy의 비헤이비어트리

	UPROPERTY()
	class UBlackboardData* BBAsset;		// Enemy의 블랙 보드
};
