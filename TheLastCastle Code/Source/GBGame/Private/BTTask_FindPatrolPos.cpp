// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "GBAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"							// UNavigationSystemV1의 GetRandomPointInNavigableRadius() 함수를 사용하기 위해서


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");		// Task 노드 이름설정
}


EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto enemy = OwnerComp.GetAIOwner()->GetPawn();		// AI가 들어가 있는 Pawn을 가져옴(= Enemy)
	if (enemy == nullptr) return EBTNodeResult::Failed;

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(enemy->GetWorld());
	if (navSystem == nullptr) return EBTNodeResult::Failed;

	FVector origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AGBAIController::HomePosKey);		// Enemy가 태어난 위치 벡터 가져옴
	FNavLocation nextPatrol;

	if (navSystem->GetRandomPointInNavigableRadius(origin, 500.0f, nextPatrol))		// Enemy가 태어난 위치에서 반경 500cm 이내 좌표를 랜덤으로 가져옴
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AGBAIController::PatrolPosKey, nextPatrol.Location);	// 랜덤 좌표를 블랙보드의 PatrolPos로 지정
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}