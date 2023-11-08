// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()


public:
	UBTTask_FindPatrolPos();	// 생성자

	// 비헤이비어 트리가 태스크 실행 시, 실행되는 함수
	// 실행 결과에 따라 컴포짓 내 다음 태스크를 계속 수행할 것인지 중지할 것인지 결정된다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
