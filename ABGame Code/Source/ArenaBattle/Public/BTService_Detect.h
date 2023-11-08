// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();	// 생성자

protected:
	// 자신이 속한 컴포짓 노드가 활성화되면 주기적으로 호출되는 함수
	// 호출 주기는 서비스 노드 내부에서 설정된 Interval 속성 값으로 지정
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
