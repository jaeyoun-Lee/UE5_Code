// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");		// ���ڷ����� ����� �̸� ����
}


// ���ڷ����� Ŭ������ CalculateRawConditionValue () �Լ��� ��ӹ޾� ������ �޼��ƴ��� �ľ��Ѵ�.
bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto enemy = OwnerComp.GetAIOwner()->GetPawn();
	if (enemy == nullptr) return false;

	auto player = Cast<AGBCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGBAIController::TargetKey));
	if (player == nullptr) return false;

	auto boss = Cast<ABoss>(enemy);
	if (boss != nullptr)
	{
		// ������ �÷��̾���� �Ÿ��� 410cm �̳����� �Ǵ��Ͽ� ����
		bResult = (player->GetDistanceTo(enemy) <= 2000.0f);
	}

	else 
	{
		// �÷��̾�� Enemy�� �Ÿ��� 200cm �̳����� �Ǵ��Ͽ� ����
		bResult = (player->GetDistanceTo(enemy) <= 200.0f);
	}

	return bResult;
}