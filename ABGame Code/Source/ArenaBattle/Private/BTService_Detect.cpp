// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect() 
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AIController에 지배중인 폰 가져오기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr) return;

	UWorld* world = ControllingPawn->GetWorld();			// AI 폰이 위치한 월드 가져옴
	FVector Center = ControllingPawn->GetActorLocation();	// AI 폰의 현재 좌표
	float DetectRadius = 600.0f;							// 탐색 반경 600cm = 6 M
	if (world == nullptr) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = world->OverlapMultiByChannel(
		OverlapResults,								// 탐색된 모든 객체를 담는 TArray
		Center,										// 현재 위치에서 탐색 시작
		FQuat::Identity,							// 충돌 검사 수행 전, 충돌 형상에 적용할 회전으로, 충돌 검사에 회전은 적용하지 않음
		ECollisionChannel::ECC_GameTraceChannel2,	// 충돌 검사에 사용할 트레이스 채널 선택
		FCollisionShape::MakeSphere(DetectRadius),	// 충돌 검사를 확인할 영역을 정의하는 충돌 모양 및 범위
		CollisionQueryParam							// 단순 충돌 모양만 고려
	);

	if (bResult)	// 무엇인가 탐색 성공 시,
	{
		for (auto const& OverlapResult : OverlapResults)	// 탐색된 오브젝트 중에서 플레이어 캐릭터 찾기
		{
			AABCharacter* player = Cast<AABCharacter>(OverlapResult.GetActor());

			// 탐색한 오브젝트가 AABCharacter 클래스이면서, 플레이어 캐릭터인가 ?
			if (player && player->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, player);	// 블랙보드 Target 오브젝트에 player 전달
				DrawDebugSphere(world, Center, DetectRadius, 16, FColor::Green, false, 0.2f);				// 발견 성공 시, 초록색 원 생성
				DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);		// 플레이어 캐릭터에게 파란 점 생성
				DrawDebugLine(world, ControllingPawn->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);	// AI 캐릭터와 플레이어 캐릭터를 파란 선으로 연결
				return;
			}
		}
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);	// 캐릭터 발견 실패 시, Target에 nullptr 전달

	// 월드에서, AI 폰의 현재 좌표를 기준으로 6미터 이내 16개의 각을 가진 빨간색 원 0.2초간 생성
	DrawDebugSphere(world, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}