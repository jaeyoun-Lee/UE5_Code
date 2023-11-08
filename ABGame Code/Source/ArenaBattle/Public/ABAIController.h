// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

	virtual void OnPossess(APawn* aPawn) override;	// 빙의하는 시점
	virtual void OnUnPossess() override;			// 빙의 해제 함수

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	void RunAI();		// AI가 동작할 수 있게 하는 함수
	void StopAI();		// AI 동작을 정지시키는 함수

private:
	void OnRepeatTimer();				// 타이머를 설치하여 지정한 시간마다 폰에게 목적지로 이동하는 명령

	FTimerHandle RepeatTimerHandle;		// 타이머
	float RepeatInterval;				// 시간 간격

	UPROPERTY()
	class UBehaviorTree* BTAsset;	// 비헤이비어 트리 애셋

	UPROPERTY()
	class UBlackboardData* BBAsset;	// 블랙보드 애셋
};
