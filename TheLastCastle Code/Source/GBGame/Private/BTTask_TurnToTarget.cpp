// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "GBEnemy.h"
#include "GBKhaimera.h"
#include "Morigesh.h"
#include "Rampage.h"
#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");	// TurnToTarget 태스크 노드 이름 설정
}


EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto enemy = Cast<AGBEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto khaimera = Cast<AGBKhaimera>(OwnerComp.GetAIOwner()->GetPawn());
	auto morigesh = Cast<AMorigesh>(OwnerComp.GetAIOwner()->GetPawn());
	auto rampage = Cast<ARampage>(OwnerComp.GetAIOwner()->GetPawn());
	auto boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy == nullptr && khaimera == nullptr && morigesh == nullptr && rampage == nullptr && boss == nullptr) return EBTNodeResult::Failed;

	auto player = Cast<AGBCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGBAIController::TargetKey));
	if (player == nullptr) return EBTNodeResult::Failed;

	if (enemy != nullptr) 
	{
		FVector lookVector = player->GetActorLocation() - enemy->GetActorLocation();
		lookVector.Z = 0.0f;

		// 지정된 X축 방향과 목표 방향을 맞추는 회전
		FRotator targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();

		// 매끄럽게 회전하기 위해서 RInterpTo() 사용
		// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
		enemy->SetActorRotation(FMath::RInterpTo(enemy->GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
	}
	
	else if (khaimera != nullptr)
	{
		FVector lookVector = player->GetActorLocation() - khaimera->GetActorLocation();
		lookVector.Z = 0.0f;

		// 지정된 X축 방향과 목표 방향을 맞추는 회전
		FRotator targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();

		// 매끄럽게 회전하기 위해서 RInterpTo() 사용
		// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
		khaimera->SetActorRotation(FMath::RInterpTo(khaimera->GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
	}

	else if (morigesh != nullptr)
	{
		FVector lookVector = player->GetActorLocation() - morigesh->GetActorLocation();
		lookVector.Z = 0.0f;

		// 지정된 X축 방향과 목표 방향을 맞추는 회전
		FRotator targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();

		// 매끄럽게 회전하기 위해서 RInterpTo() 사용
		// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
		morigesh->SetActorRotation(FMath::RInterpTo(morigesh->GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
	}

	if (rampage != nullptr)
	{
		FVector lookVector = player->GetActorLocation() - rampage->GetActorLocation();
		lookVector.Z = 0.0f;

		// 지정된 X축 방향과 목표 방향을 맞추는 회전
		FRotator targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();

		// 매끄럽게 회전하기 위해서 RInterpTo() 사용
		// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
		rampage->SetActorRotation(FMath::RInterpTo(rampage->GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
	}

	if (boss != nullptr)
	{
		FVector lookVector = player->GetActorLocation() - boss->GetActorLocation();
		lookVector.Z = 0.0f;

		// 지정된 X축 방향과 목표 방향을 맞추는 회전
		FRotator targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();

		// 매끄럽게 회전하기 위해서 RInterpTo() 사용
		// 현재 회전 값, 목표 회전 값, 델타시간, 보간중 허용되는 최대 변화율
		boss->SetActorRotation(FMath::RInterpTo(boss->GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
	}
	return EBTNodeResult::Succeeded;
}
