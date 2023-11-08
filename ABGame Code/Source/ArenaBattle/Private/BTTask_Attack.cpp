// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	// Tick ��� Ȱ��ȭ
	bNotifyTick = true;
	isAttacking = false;
	NodeName = TEXT("NPC Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto NPC = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (NPC == nullptr) return EBTNodeResult::Failed;

	NPC->Attack();	// AABCharacter�� Attack()�Լ� ȣ��
	isAttacking = true;

	// ���� ��Ÿ�ְ� ������ �˷��ִ� ABCharacter�� OnAttackMontageEnded() �Լ����� ��������Ʈ ȣ��
	NPC->OnAttackEnd.AddLambda([this]()-> void {	
		isAttacking = false;
		});

	return EBTNodeResult::InProgress;	// �½�ũ�� ��� �����ϰ� �ִ� ��, �½�ũ ���� ����� ���� �˷��� ����
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
	if (!isAttacking)	// ���� ���̾��ٰ� ���� ���� �ƴϰ� �Ǹ�,
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);	// �½�ũ�� �����ٰ� �˷��ִ� �Լ�
	}
}