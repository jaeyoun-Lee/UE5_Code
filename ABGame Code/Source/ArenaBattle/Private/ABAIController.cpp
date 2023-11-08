// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	// �����Ϳ��� ���� ������ �ּ� ���
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObj(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBObj.Succeeded())
	{
		BBAsset = BBObj.Object;
	}

	// �����Ϳ��� ���� �����̺�� Ʈ�� �ּ� ���
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObj(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTObj.Succeeded())
	{
		BTAsset = BTObj.Object;
	}

	//RepeatInterval = 3.0f;	// �ݺ��� �ð� ����
}

//	�����ϴ� ������ ȣ��
void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	// �� �ڵ�� RunAI() �Լ��� �̵�
	// UseBlackboard���� �Ű������� UBlackboardComponent* Ÿ���� �ʿ��ϴ�.
	// ����, Blackboard�� �Ű������� �־��ָ� ������ �߻��ϹǷ� �Ʒ��� ���� ���� �ϳ��� ������ش�.
	//UBlackboardComponent* bbComponent = Blackboard;

	//if (UseBlackboard(BBAsset, bbComponent))	// �� ���� ���
	//{
	//	bbComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());	// ���� ������ Ű�� ����
	//	if (!RunBehaviorTree(BTAsset))	// �����̺�� Ʈ�� ����
	//	{
	//		ABLOG(Error, TEXT("AIController Couldn't run behavior tree!"));
	//	}
	//}
	//// ������ Ÿ�̸�(= RepeatTimerHandle)��, ������ �ð�(= RepeatInterval)��ŭ OnRepeatTimer() �Լ� Loop(�ݺ�)
	//GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
}

void AABAIController::RunAI()
{
	// HomePosKeyŰ ���� ���� NPC ��ġ�� ���������ν�, NPC�� �̵��� �� �ֵ��� ����
	// BTTask_FindPatrolPos.cpp���� HomePosKey(= ĳ���� ���� ��ġ)�� Ű������ �ݰ� 500cm�̳� ��ǥ ���Ƿ� ���� ��, NextPatrol�� �����ϱ� ����
	UBlackboardComponent* bbComponent = Blackboard;
	if (UseBlackboard(BBAsset, bbComponent))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))	// �����̺��Ʈ���� �����ϴ��� ?
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree !"));
		}
	}
}

void AABAIController::StopAI()
{
	// �����̺�� Ʈ���� ������ ����ϴ� ���� ���
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);	

	if (BehaviorTreeComponent != nullptr)
	{
		// AI�� ������Ű�� �Լ�(EBTStopMode���� Safe(����) / Forced(����)�� ��尡 �ִ�.)
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

// ���ǰ� ������ ��
void AABAIController::OnUnPossess()
{
	Super::OnUnPossess();
	// Ÿ�̸Ӹ� ������Ű��, �ش� �ڵ��� ��ȿȭ
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);	
}

void AABAIController::OnRepeatTimer()
{
	auto CurrentPawn = GetPawn();
	ABCHECK(CurrentPawn != nullptr);

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) return;

	FNavLocation NextLocation;
	
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
		ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
	}
}

