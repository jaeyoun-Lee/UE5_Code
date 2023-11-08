// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	// Tick 기능 활성화
	bNotifyTick = true;
	isAttacking = false;
	NodeName = TEXT("NPC Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto NPC = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (NPC == nullptr) return EBTNodeResult::Failed;

	NPC->Attack();	// AABCharacter의 Attack()함수 호출
	isAttacking = true;

	// 공격 몽타주가 끝남을 알려주는 ABCharacter의 OnAttackMontageEnded() 함수에서 델리게이트 호출
	NPC->OnAttackEnd.AddLambda([this]()-> void {	
		isAttacking = false;
		});

	return EBTNodeResult::InProgress;	// 태스크를 계속 수행하고 있는 중, 태스크 실행 결과는 향후 알려줄 예정
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
	if (!isAttacking)	// 공격 중이었다가 공격 중이 아니게 되면,
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);	// 태스크가 끝났다고 알려주는 함수
	}
}