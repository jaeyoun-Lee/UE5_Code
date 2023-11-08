// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");		// Service노드 이름 설정
	Interval = 1.0f;				// 시간 간격 1초로 설정
}


void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* enemy = OwnerComp.GetAIOwner()->GetPawn();
	if (enemy == nullptr) return;

	UWorld* world = enemy->GetWorld();				// 탐색할 곳(enemy가 위치한 월드)
	FVector center = enemy->GetActorLocation();		// 탐색 중점은 enemy의 현재 위치
	float detectRadius = 600.0f;					// 탐색 반경 600cm
	if (world == nullptr) return;

	auto boss = Cast<ABoss>(enemy);
	if (boss != nullptr)
	{
		detectRadius = boss->detectRange;
	}

	TArray<FOverlapResult> overlapResults;	// 탐색된 액터를 모두 저장할 Tarray 타입의 overlapResult
	FCollisionQueryParams collisionQueryParam(NAME_None, false, enemy);		// 단순 충돌 모양만 고려

	bool bResult = world->OverlapMultiByChannel(
		overlapResults,									// 탐색된 액터를 모두 저장할 곳
		center,											// 탐색 중점은 enemy 현재 위치
		FQuat::Identity,								// 충돌 검사에 회전 적용 X
		ECollisionChannel::ECC_GameTraceChannel2,		// Attack 트레이스로 탐지
		FCollisionShape::MakeSphere(detectRadius),		// 구체로 모양으로 탐지
		collisionQueryParam								// 단순 충돌 모양만 고려
	);

	
	if (bResult)
	{
		if (boss != nullptr)		// 보스이면 캐릭터의 위치까지 전달
		{
			for (auto const& overlapResult : overlapResults)
			{
				AGBCharacter* player = Cast<AGBCharacter>(overlapResult.GetActor());	// 오버랩 된 액터중 플레이어가 있는지 캐스팅

				if (player && player->GetController()->IsPlayerController() && !player->isDeath)		// 캐스팅 성공 및 이 게임의 플레이어인지 확인
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, player);					// 블랙보드의 Target에 플레이어 저장
					boss->TargetLocation = player->GetActorLocation();				// 플레이어의 현재 위치 전달

					DrawDebugSphere(world, center, detectRadius, 16, FColor::Green, false, 0.3f);								// 플레이어 탐지 시, 초록색 원 생성
					DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);						// 플레이어 탐지 시, 플레이어에게 파란색 점 생성
					DrawDebugLine(world, enemy->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);		// 플레이어 탐지 시, 플레이어와 Enemy를 연결하는 파란 선 생성
					return;
				}
			}
		}

		else
		{
			for (auto const& overlapResult : overlapResults)
			{
				AGBCharacter* player = Cast<AGBCharacter>(overlapResult.GetActor());	// 오버랩 된 액터중 플레이어가 있는지 캐스팅

				if (player && player->GetController()->IsPlayerController() && !player->isDeath)		// 캐스팅 성공 및 이 게임의 플레이어인지 확인
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, player);					// 블랙보드의 Target에 플레이어 저장

					DrawDebugSphere(world, center, detectRadius, 16, FColor::Green, false, 0.3f);								// 플레이어 탐지 시, 초록색 원 생성
					DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);						// 플레이어 탐지 시, 플레이어에게 파란색 점 생성
					DrawDebugLine(world, enemy->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);		// 플레이어 탐지 시, 플레이어와 Enemy를 연결하는 파란 선 생성
					return;
				}
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, nullptr);	// 캐릭터 발견 실패 시, Target에 nullptr 전달

	// Player 탐지 못하면, 월드에서, AI 폰의 현재 좌표를 기준으로 6미터 이내 16개의 각을 가진 빨간색 원 0.2초간 생성
	DrawDebugSphere(world, center, detectRadius, 16, FColor::Red, false, 0.2f);
}