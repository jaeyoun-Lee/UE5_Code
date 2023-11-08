// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "GBAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"							// UNavigationSystemV1�� GetRandomPointInNavigableRadius() �Լ��� ����ϱ� ���ؼ�


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");		// Task ��� �̸�����
}


EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto enemy = OwnerComp.GetAIOwner()->GetPawn();		// AI�� �� �ִ� Pawn�� ������(= Enemy)
	if (enemy == nullptr) return EBTNodeResult::Failed;

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(enemy->GetWorld());
	if (navSystem == nullptr) return EBTNodeResult::Failed;

	FVector origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AGBAIController::HomePosKey);		// Enemy�� �¾ ��ġ ���� ������
	FNavLocation nextPatrol;

	if (navSystem->GetRandomPointInNavigableRadius(origin, 500.0f, nextPatrol))		// Enemy�� �¾ ��ġ���� �ݰ� 500cm �̳� ��ǥ�� �������� ������
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AGBAIController::PatrolPosKey, nextPatrol.Location);	// ���� ��ǥ�� �������� PatrolPos�� ����
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}