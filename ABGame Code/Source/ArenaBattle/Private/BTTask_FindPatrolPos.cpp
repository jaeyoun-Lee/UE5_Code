// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	// 태스크의 이름을 다른 이름으로 표시하고 싶을때 NodeName 속성을 다른 값으로 지정
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();	// 빙의한 폰 찾기
	if (ControllingPawn == nullptr) return EBTNodeResult::Failed;	// 못찾으면 실패 반환

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());	// 내비 시스템 찾기
	if (NavSystem == nullptr) return EBTNodeResult::Failed;													// 못찾으면 실패 반환

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey);		// HomePosKey의 저장된 키값(좌표)를 Origin변수에 저장
	FNavLocation NextPatrol;

	// HomePosKey(= 캐릭터 현재 위치)의 키값부터 반경 500cm이내 좌표 임의로 추출 후, NextPatrol에 저장
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrol))		
	{
		// 임의의 좌표를 PatrolPosKey의 키값으로 저장(다음 이동 좌표를 의미)
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
