// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsInAttackRange();

protected:
	// ���ڷ����� Ŭ������ CalculateRawConditionValue () �Լ��� ��ӹ޾� ������ �޼��ƴ��� �ľ��Ѵ�.
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
