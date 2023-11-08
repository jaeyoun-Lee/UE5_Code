// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();

	// �����̺�� Ʈ���� �½�ũ�� ������ ��, �½�ũ Ŭ������ ExecuteTask() �Լ��� �����Ѵ�.
	// ExecuteTask() �Լ��� �ݵ�� �½�ũ�� ������ ���(����, ����, ���߿� �˷���)�� ��ȯ�ؾ��Ѵ�.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool isAttacking = false;
};
