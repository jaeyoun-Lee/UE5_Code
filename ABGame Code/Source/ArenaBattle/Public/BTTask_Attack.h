// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();	// 생성자
	
	// 비헤이비어 트리가 태스크 실행 시, 실행되는 함수
	// 실행 결과에 따라 컴포짓 내 다음 태스크를 계속 수행할 것인지 중지할 것인지 결정된다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 자신이 속한 컴포짓 노드가 활성화되면 주기적으로 호출되는 함수
	// 호출 주기는 서비스 노드 내부에서 설정된 Interval 속성 값으로 지정
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// 공격 중인지 ?
	bool isAttacking = false;
};
