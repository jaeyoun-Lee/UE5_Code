// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();	// ������

protected:
	// �ڽ��� ���� ������ ��尡 Ȱ��ȭ�Ǹ� �ֱ������� ȣ��Ǵ� �Լ�
	// ȣ�� �ֱ�� ���� ��� ���ο��� ������ Interval �Ӽ� ������ ����
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
