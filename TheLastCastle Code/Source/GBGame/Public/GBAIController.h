// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "AIController.h"
#include "GBAIController.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API AGBAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGBAIController();		// ������

	virtual void OnPossess(APawn* InPawn) override;		// �����ϴ� ����
	virtual void OnUnPossess() override;				// ���� ���� �Լ�

	static const FName HomePosKey;						// Enemy ���� ����	
	static const FName PatrolPosKey;					// ������ ��ġ ����
	static const FName TargetKey;						// Target(= Player) ������Ʈ�� ������ ObjectŸ��

	void RunAI();		// AI�� ������ �� �ְ� �ϴ� �Լ�
	void StopAI();		// AI ������ ������Ű�� �Լ�

private:
	void OnRepeatTimer();				// Ÿ�̸Ӹ� ��ġ�Ͽ� ������ �ð����� ������ �������� �̵��ϴ� ���

	FTimerHandle RepeatTimerHandle;		// Ÿ�̸�
	float repeatInterval;				// �ð� ����

	UPROPERTY()	
	class UBehaviorTree* BTAsset;		// Enemy�� �����̺��Ʈ��

	UPROPERTY()
	class UBlackboardData* BBAsset;		// Enemy�� �� ����
};
