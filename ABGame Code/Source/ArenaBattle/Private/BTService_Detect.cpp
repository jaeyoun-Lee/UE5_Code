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

	// AIController�� �������� �� ��������
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr) return;

	UWorld* world = ControllingPawn->GetWorld();			// AI ���� ��ġ�� ���� ������
	FVector Center = ControllingPawn->GetActorLocation();	// AI ���� ���� ��ǥ
	float DetectRadius = 600.0f;							// Ž�� �ݰ� 600cm = 6 M
	if (world == nullptr) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = world->OverlapMultiByChannel(
		OverlapResults,								// Ž���� ��� ��ü�� ��� TArray
		Center,										// ���� ��ġ���� Ž�� ����
		FQuat::Identity,							// �浹 �˻� ���� ��, �浹 ���� ������ ȸ������, �浹 �˻翡 ȸ���� �������� ����
		ECollisionChannel::ECC_GameTraceChannel2,	// �浹 �˻翡 ����� Ʈ���̽� ä�� ����
		FCollisionShape::MakeSphere(DetectRadius),	// �浹 �˻縦 Ȯ���� ������ �����ϴ� �浹 ��� �� ����
		CollisionQueryParam							// �ܼ� �浹 ��縸 ���
	);

	if (bResult)	// �����ΰ� Ž�� ���� ��,
	{
		for (auto const& OverlapResult : OverlapResults)	// Ž���� ������Ʈ �߿��� �÷��̾� ĳ���� ã��
		{
			AABCharacter* player = Cast<AABCharacter>(OverlapResult.GetActor());

			// Ž���� ������Ʈ�� AABCharacter Ŭ�����̸鼭, �÷��̾� ĳ�����ΰ� ?
			if (player && player->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, player);	// ������ Target ������Ʈ�� player ����
				DrawDebugSphere(world, Center, DetectRadius, 16, FColor::Green, false, 0.2f);				// �߰� ���� ��, �ʷϻ� �� ����
				DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);		// �÷��̾� ĳ���Ϳ��� �Ķ� �� ����
				DrawDebugLine(world, ControllingPawn->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);	// AI ĳ���Ϳ� �÷��̾� ĳ���͸� �Ķ� ������ ����
				return;
			}
		}
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);	// ĳ���� �߰� ���� ��, Target�� nullptr ����

	// ���忡��, AI ���� ���� ��ǥ�� �������� 6���� �̳� 16���� ���� ���� ������ �� 0.2�ʰ� ����
	DrawDebugSphere(world, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}