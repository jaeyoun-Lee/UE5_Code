// Fill out your copyright notice in the Description page of Project Settings.


#include "GBAIController.h"
#include "NavigationSystem.h"							// UNavigationSystemV1�� GetRandomPointInNavigableRadius() �Լ��� ����ϱ� ���ؼ�
#include "Blueprint/AIBlueprintHelperLibrary.h"			// �̵� ������ �������� �������� �������� GetRandomPointInNavigableRadius() �Լ��� ����ϱ� ���ؼ�
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


const FName AGBAIController::HomePosKey(TEXT("HomePos"));		// �������� Ű �̸� HomePos�� FName HomePosKey���� ����
const FName AGBAIController::PatrolPosKey(TEXT("PatrolPos"));	// �������� Ű �̸� PatrolPos�� FName PatrolPosKey���� ����
const FName AGBAIController::TargetKey(TEXT("Target"));			// �������� Ű �̸� Target�� FName TargetKey���� ����	

AGBAIController::AGBAIController()
{
	repeatInterval = 2.0f;	// Ÿ�̸� �ݺ��ϴ� ���̿� �ð� �� 3��

	static ConstructorHelpers::FObjectFinder<UBlackboardData> bb_obj(TEXT("/Script/AIModule.BlackboardData'/Game/BP/AI/BB_Enemy.BB_Enemy'"));
	if(bb_obj.Succeeded())
	{
		BBAsset = bb_obj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> bt_obj(TEXT("/Script/AIModule.BehaviorTree'/Game/BP/AI/BT_Enemy.BT_Enemy'"));
	if (bt_obj.Succeeded())
	{
		BTAsset = bt_obj.Object;
	}

}

void AGBAIController::OnPossess(APawn* InPawn)	// ���� �ϴ� ����
{
	Super::OnPossess(InPawn);

}

void AGBAIController::RunAI()
{
	// Ÿ�̸� �ڵ��, OnRepeatTimer() �Լ� ����, 3�ʸ���, �ݺ��ؼ�
	//GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AGBAIController::OnRepeatTimer, repeatInterval, true);
	
	// UseBlackboard���� �Ű� ������ UBlackboard* Ÿ���� �ʿ��ϴ�.
	// ����, Blackboard�� �Ű������� �־��ָ� ������ �߻��Ѵ�. �׷��Ƿ� �Ʒ�ó�� ������ �ϳ� ������ش�.
	UBlackboardComponent* blackboard = Blackboard;
	if (UseBlackboard(BBAsset, blackboard))		// �����̺�� Ʈ�� ���°� ���� ������ ��ġ�� ������ �ּ��� �Բ� �����Ѵ�.
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());		// Enemy�� ���� ��ġ�� �������� HomePos ���Ϳ� ����

		if (!RunBehaviorTree(BTAsset))
		{
			GBLOG(Error, TEXT("AI Controller couldn't  run behavior tree"));
		}
	}
}

void AGBAIController::StopAI()
{
	// �����̺�� Ʈ���� ������ ����ϴ� ���� ���
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BehaviorTreeComponent != nullptr)
	{
		// AI�� ������Ű�� �Լ�(EBTStopMode���� Safe(����) / Forced(����)�� ��尡 �ִ�.)
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

void AGBAIController::OnUnPossess()
{
	Super::OnUnPossess();
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);	// Ÿ�̸Ӹ� ������Ű��, �ش� �ڵ��� ��ȿȭ
}


// OnRepeatTimer�� 3�ʿ� �ѹ��� �ݺ��� �Լ�
void AGBAIController::OnRepeatTimer()
{
	auto currentPawn = GetPawn();		// AIController�� ���ִ� Enemy�� ������
	GBCHECK(currentPawn != nullptr);

	// GetRandomPointInNavigableRadius() �Լ��� ����ϱ� ���ؼ� UNavigationSystemV1 ����
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) return;

	FNavLocation nextLocation;	// �̵��� ��ġ�� ������ ����

	// �𸮾� ������ ������̼� �ý������� �̵� ������ �������� �������� �������� GetRandomPointInNavigableRadius()�Լ��� ������ �������� nextLocation�� ����
	// ���� �ֺ��� �ݰ����� 500cm �� �������� ������ ������ ã��
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, nextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, nextLocation.Location);
		GBLOG(Warning, TEXT("Next Location : %s"), *nextLocation.Location.ToString());
	}
}