// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

	virtual void OnPossess(APawn* aPawn) override;	// �����ϴ� ����
	virtual void OnUnPossess() override;			// ���� ���� �Լ�

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	void RunAI();		// AI�� ������ �� �ְ� �ϴ� �Լ�
	void StopAI();		// AI ������ ������Ű�� �Լ�

private:
	void OnRepeatTimer();				// Ÿ�̸Ӹ� ��ġ�Ͽ� ������ �ð����� ������ �������� �̵��ϴ� ���

	FTimerHandle RepeatTimerHandle;		// Ÿ�̸�
	float RepeatInterval;				// �ð� ����

	UPROPERTY()
	class UBehaviorTree* BTAsset;	// �����̺�� Ʈ�� �ּ�

	UPROPERTY()
	class UBlackboardData* BBAsset;	// ������ �ּ�
};
