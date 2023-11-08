// Fill out your copyright notice in the Description page of Project Settings.


#include "GBAIController.h"
#include "NavigationSystem.h"							// UNavigationSystemV1의 GetRandomPointInNavigableRadius() 함수를 사용하기 위해서
#include "Blueprint/AIBlueprintHelperLibrary.h"			// 이동 가능한 목적지를 랜덤으로 가져오는 GetRandomPointInNavigableRadius() 함수를 사용하기 위해서
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


const FName AGBAIController::HomePosKey(TEXT("HomePos"));		// 블랙보드의 키 이름 HomePos를 FName HomePosKey으로 설정
const FName AGBAIController::PatrolPosKey(TEXT("PatrolPos"));	// 블랙보드의 키 이름 PatrolPos를 FName PatrolPosKey으로 설정
const FName AGBAIController::TargetKey(TEXT("Target"));			// 블랙보드의 키 이름 Target를 FName TargetKey으로 설정	

AGBAIController::AGBAIController()
{
	repeatInterval = 2.0f;	// 타이머 반복하는 사이에 시간 텀 3초

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

void AGBAIController::OnPossess(APawn* InPawn)	// 빙의 하는 시점
{
	Super::OnPossess(InPawn);

}

void AGBAIController::RunAI()
{
	// 타이머 핸들로, OnRepeatTimer() 함수 실행, 3초마다, 반복해서
	//GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AGBAIController::OnRepeatTimer, repeatInterval, true);
	
	// UseBlackboard에서 매개 변수로 UBlackboard* 타입이 필요하다.
	// 따라서, Blackboard로 매개변수를 주어주면 에러가 발생한다. 그러므로 아래처럼 변수를 하나 만들어준다.
	UBlackboardComponent* blackboard = Blackboard;
	if (UseBlackboard(BBAsset, blackboard))		// 비헤이비어 트리 에셋과 같은 폴더에 위치한 블랙보드 애셋이 함께 동작한다.
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());		// Enemy의 현재 위치를 블랙보드의 HomePos 벡터에 저장

		if (!RunBehaviorTree(BTAsset))
		{
			GBLOG(Error, TEXT("AI Controller couldn't  run behavior tree"));
		}
	}
}

void AGBAIController::StopAI()
{
	// 비헤이비어 트리의 동작을 담당하는 구성 요소
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BehaviorTreeComponent != nullptr)
	{
		// AI를 정지시키는 함수(EBTStopMode에는 Safe(안전) / Forced(강제)의 모드가 있다.)
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

void AGBAIController::OnUnPossess()
{
	Super::OnUnPossess();
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);	// 타이머를 중지시키고, 해당 핸들을 무효화
}


// OnRepeatTimer로 3초에 한번씩 반복될 함수
void AGBAIController::OnRepeatTimer()
{
	auto currentPawn = GetPawn();		// AIController가 들어가있는 Enemy를 가져옴
	GBCHECK(currentPawn != nullptr);

	// GetRandomPointInNavigableRadius() 함수를 사용하기 위해서 UNavigationSystemV1 생성
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) return;

	FNavLocation nextLocation;	// 이동할 위치를 저장할 변수

	// 언리얼 엔진의 내비게이션 시스템으로 이동 가능한 목적지를 랜덤으로 가져오는 GetRandomPointInNavigableRadius()함수로 목적지 가져오고 nextLocation에 저장
	// 원점 주변의 반경으로 500cm 내 공간에서 무작위 지점을 찾음
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, nextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, nextLocation.Location);
		GBLOG(Warning, TEXT("Next Location : %s"), *nextLocation.Location.ToString());
	}
}