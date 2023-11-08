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

	// AI Controller에 빙의된 폰을 가져옴
	auto NPC = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (NPC == nullptr) return EBTNodeResult::Failed;

	// 블랙보드 Target오브젝트에 플레이어의 값이 들어가 있는지(= 플레이어가 탐지되었는지 ?) ?
	auto player = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (player == nullptr) return EBTNodeResult::Failed;

	FVector LookVector = player->GetActorLocation() - NPC->GetActorLocation();
	LookVector.Z = 0.0f;

	// 지정된 X축 방향과 목표 방향을 맞추는 회전
	FRotator playerRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	// 매끄럽게 회전하기 위해서 RInterpTo() 사용
	// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
	NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(), playerRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}