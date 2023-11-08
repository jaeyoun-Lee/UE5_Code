// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	// �½�ũ�� �̸��� �ٸ� �̸����� ǥ���ϰ� ������ NodeName �Ӽ��� �ٸ� ������ ����
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();	// ������ �� ã��
	if (ControllingPawn == nullptr) return EBTNodeResult::Failed;	// ��ã���� ���� ��ȯ

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());	// ���� �ý��� ã��
	if (NavSystem == nullptr) return EBTNodeResult::Failed;													// ��ã���� ���� ��ȯ

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey);		// HomePosKey�� ����� Ű��(��ǥ)�� Origin������ ����
	FNavLocation NextPatrol;

	// HomePosKey(= ĳ���� ���� ��ġ)�� Ű������ �ݰ� 500cm�̳� ��ǥ ���Ƿ� ���� ��, NextPatrol�� ����
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrol))		
	{
		// ������ ��ǥ�� PatrolPosKey�� Ű������ ����(���� �̵� ��ǥ�� �ǹ�)
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
