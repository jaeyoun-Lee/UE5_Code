// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");		// 데코레이터 노드의 이름 설정
}


// 데코레이터 클래스는 CalculateRawConditionValue () 함수를 상속받아 조건이 달성됐는지 파악한다.
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
		// 보스는 플레이어와의 거리가 410cm 이내인지 판단하여 리턴
		bResult = (player->GetDistanceTo(enemy) <= 2000.0f);
	}

	else 
	{
		// 플레이어와 Enemy의 거리가 200cm 이내인지 판단하여 리턴
		bResult = (player->GetDistanceTo(enemy) <= 200.0f);
	}

	return bResult;
}