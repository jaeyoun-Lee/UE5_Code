// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	
	auto ControllingPawn = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());	// NPC �� ��������
	if (ControllingPawn == nullptr) return false;

	// ���� Target�� ����Ǿ��ִ� ���� �÷��̾��� ĳ�������� ?
	auto target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (target == nullptr) return false;

	// NPC���� �÷��̾� �� �� �� ĳ�����ϰ�, �� ĳ���� ������ �Ÿ��� 200 �����̸� true ��ȯ(= ���� �����̺�� Ʈ�� ����)
	bResult = (target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFinalAttackRange());
	return bResult;
}