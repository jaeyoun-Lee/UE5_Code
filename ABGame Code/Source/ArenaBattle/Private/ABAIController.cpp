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
	// 에디터에서 만든 블랙보드 애셋 등록
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObj(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBObj.Succeeded())
	{
		BBAsset = BBObj.Object;
	}

	// 에디터에서 만든 비헤이비어 트리 애셋 등록
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObj(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTObj.Succeeded())
	{
		BTAsset = BTObj.Object;
	}

	//RepeatInterval = 3.0f;	// 반복할 시간 간격
}

//	빙의하는 시점에 호출
void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	// 이 코드는 RunAI() 함수로 이동
	// UseBlackboard에서 매개변수로 UBlackboardComponent* 타입이 필요하다.
	// 따라서, Blackboard로 매개변수를 주어주면 에러가 발생하므로 아래와 같이 변수 하나를 만들어준다.
	//UBlackboardComponent* bbComponent = Blackboard;

	//if (UseBlackboard(BBAsset, bbComponent))	// 블랙 보드 사용
	//{
	//	bbComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());	// 벡터 값으로 키값 설정
	//	if (!RunBehaviorTree(BTAsset))	// 비헤이비어 트리 실행
	//	{
	//		ABLOG(Error, TEXT("AIController Couldn't run behavior tree!"));
	//	}
	//}
	//// 지정한 타이머(= RepeatTimerHandle)로, 지정한 시간(= RepeatInterval)만큼 OnRepeatTimer() 함수 Loop(반복)
	//GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
}

void AABAIController::RunAI()
{
	// HomePosKey키 값을 현재 NPC 위치로 설정함으로써, NPC가 이동할 수 있도록 해줌
	// BTTask_FindPatrolPos.cpp에서 HomePosKey(= 캐릭터 현재 위치)의 키값부터 반경 500cm이내 좌표 임의로 추출 후, NextPatrol에 저장하기 때문
	UBlackboardComponent* bbComponent = Blackboard;
	if (UseBlackboard(BBAsset, bbComponent))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))	// 비헤이비어트리가 동작하는지 ?
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree !"));
		}
	}
}

void AABAIController::StopAI()
{
	// 비헤이비어 트리의 동작을 담당하는 구성 요소
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);	

	if (BehaviorTreeComponent != nullptr)
	{
		// AI를 정지시키는 함수(EBTStopMode에는 Safe(안전) / Forced(강제)의 모드가 있다.)
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

// 빙의가 해제될 때
void AABAIController::OnUnPossess()
{
	Super::OnUnPossess();
	// 타이머를 중지시키고, 해당 핸들을 무효화
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

