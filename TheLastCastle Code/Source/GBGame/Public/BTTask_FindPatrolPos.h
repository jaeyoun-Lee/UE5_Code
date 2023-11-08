// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindPatrolPos();

	// 비헤이비어 트리는 태스크를 실행할 때, 태스크 클래스의 ExecuteTask() 함수를 실행한다.
	// ExecuteTask() 함수는 반드시 태스크를 수행한 결과(실패, 성공, 나중에 알려줌)를 반환해야한다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
