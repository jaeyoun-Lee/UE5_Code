// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "GBEnemy.h"
#include "GBEnemyAnimInstance.h"
#include "GBKhaimera.h"
#include "KhaimeraAnimInstance.h"
#include "Morigesh.h"
#include "MorigeshAnimInstance.h"
#include "Rampage.h"
#include "RampageAnimInstance.h"
#include "Boss.h"
#include "BossAnimInstance.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;		// TickTask 사용하기 위해 선언
	isAttacking = false;
}


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto enemy = Cast<AGBEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto khaimera = Cast<AGBKhaimera>(OwnerComp.GetAIOwner()->GetPawn());
	auto morigesh = Cast<AMorigesh>(OwnerComp.GetAIOwner()->GetPawn());
	auto rampage = Cast<ARampage>(OwnerComp.GetAIOwner()->GetPawn());
	auto boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy == nullptr && khaimera == nullptr && morigesh == nullptr && rampage == nullptr && boss == nullptr) return EBTNodeResult::Failed;

	if (enemy != nullptr)
	{
		enemy->Attack();
		isAttacking = true;

		auto enemyAnim = Cast<UGBEnemyAnimInstance>(enemy->GetMesh()->GetAnimInstance());

		enemyAnim->OnResetCheck.AddLambda([this]() -> void {
			isAttacking = false;
			});
	}
	
	if (khaimera != nullptr)
	{
		khaimera->Attack();
		isAttacking = true;

		auto khaimerayAnim = Cast<UKhaimeraAnimInstance>(khaimera->GetMesh()->GetAnimInstance());
		
		khaimerayAnim->OnResetCheck.AddLambda([this]() -> void {
			isAttacking = false;
			});
	}


	if (morigesh != nullptr)
	{
		morigesh->Attack();
		isAttacking = true;

		auto morigeshAnim = Cast<UMorigeshAnimInstance>(morigesh->GetMesh()->GetAnimInstance());

		morigeshAnim->OnResetCheck.AddLambda([this]() -> void {
			isAttacking = false;
			});
	}

	if (rampage != nullptr)
	{
		rampage->Attack();
		isAttacking = true;

		auto rampageAnim = Cast<URampageAnimInstance>(rampage->GetMesh()->GetAnimInstance());

		rampageAnim->OnResetCheck.AddLambda([this]() -> void {
			isAttacking = false;
			});
	}
	
	if (boss != nullptr)
	{
		boss->Attack();
		isAttacking = true;

		auto bossAnim = Cast<UBossAnimInstance>(boss->GetMesh()->GetAnimInstance());

		bossAnim->OnResetCheck.AddLambda([this]() -> void {
			isAttacking = false;
			});
	}

	return EBTNodeResult::InProgress;	// 태스크를 계속 수행하고 있는 중, 태스크 실행 결과는 향후 알려줄 예정
}


void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!isAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);		// Succeeded를 전달하여, 태스크가 끝났다고 알려주는 함수
	}
}