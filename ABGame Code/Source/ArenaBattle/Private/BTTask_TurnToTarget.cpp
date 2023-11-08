// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn to Target");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI Controller�� ���ǵ� ���� ������
	auto NPC = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (NPC == nullptr) return EBTNodeResult::Failed;

	// ������ Target������Ʈ�� �÷��̾��� ���� �� �ִ���(= �÷��̾ Ž���Ǿ����� ?) ?
	auto player = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (player == nullptr) return EBTNodeResult::Failed;

	FVector LookVector = player->GetActorLocation() - NPC->GetActorLocation();
	LookVector.Z = 0.0f;

	// ������ X�� ����� ��ǥ ������ ���ߴ� ȸ��
	FRotator playerRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	// �Ų����� ȸ���ϱ� ���ؼ� RInterpTo() ���
	// ���� ȸ�� ��, ��ǥ ȸ�� ��, ��Ÿ�ð�, ������ ���Ǵ� �ִ� ��ȭ��
	NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(), playerRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}