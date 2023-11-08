// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "GBAIController.h"
#include "GBCharacter.h"
#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");		// Service��� �̸� ����
	Interval = 1.0f;				// �ð� ���� 1�ʷ� ����
}


void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* enemy = OwnerComp.GetAIOwner()->GetPawn();
	if (enemy == nullptr) return;

	UWorld* world = enemy->GetWorld();				// Ž���� ��(enemy�� ��ġ�� ����)
	FVector center = enemy->GetActorLocation();		// Ž�� ������ enemy�� ���� ��ġ
	float detectRadius = 600.0f;					// Ž�� �ݰ� 600cm
	if (world == nullptr) return;

	auto boss = Cast<ABoss>(enemy);
	if (boss != nullptr)
	{
		detectRadius = boss->detectRange;
	}

	TArray<FOverlapResult> overlapResults;	// Ž���� ���͸� ��� ������ Tarray Ÿ���� overlapResult
	FCollisionQueryParams collisionQueryParam(NAME_None, false, enemy);		// �ܼ� �浹 ��縸 ���

	bool bResult = world->OverlapMultiByChannel(
		overlapResults,									// Ž���� ���͸� ��� ������ ��
		center,											// Ž�� ������ enemy ���� ��ġ
		FQuat::Identity,								// �浹 �˻翡 ȸ�� ���� X
		ECollisionChannel::ECC_GameTraceChannel2,		// Attack Ʈ���̽��� Ž��
		FCollisionShape::MakeSphere(detectRadius),		// ��ü�� ������� Ž��
		collisionQueryParam								// �ܼ� �浹 ��縸 ���
	);

	
	if (bResult)
	{
		if (boss != nullptr)		// �����̸� ĳ������ ��ġ���� ����
		{
			for (auto const& overlapResult : overlapResults)
			{
				AGBCharacter* player = Cast<AGBCharacter>(overlapResult.GetActor());	// ������ �� ������ �÷��̾ �ִ��� ĳ����

				if (player && player->GetController()->IsPlayerController() && !player->isDeath)		// ĳ���� ���� �� �� ������ �÷��̾����� Ȯ��
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, player);					// �������� Target�� �÷��̾� ����
					boss->TargetLocation = player->GetActorLocation();				// �÷��̾��� ���� ��ġ ����

					DrawDebugSphere(world, center, detectRadius, 16, FColor::Green, false, 0.3f);								// �÷��̾� Ž�� ��, �ʷϻ� �� ����
					DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);						// �÷��̾� Ž�� ��, �÷��̾�� �Ķ��� �� ����
					DrawDebugLine(world, enemy->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);		// �÷��̾� Ž�� ��, �÷��̾�� Enemy�� �����ϴ� �Ķ� �� ����
					return;
				}
			}
		}

		else
		{
			for (auto const& overlapResult : overlapResults)
			{
				AGBCharacter* player = Cast<AGBCharacter>(overlapResult.GetActor());	// ������ �� ������ �÷��̾ �ִ��� ĳ����

				if (player && player->GetController()->IsPlayerController() && !player->isDeath)		// ĳ���� ���� �� �� ������ �÷��̾����� Ȯ��
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, player);					// �������� Target�� �÷��̾� ����

					DrawDebugSphere(world, center, detectRadius, 16, FColor::Green, false, 0.3f);								// �÷��̾� Ž�� ��, �ʷϻ� �� ����
					DrawDebugPoint(world, player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);						// �÷��̾� Ž�� ��, �÷��̾�� �Ķ��� �� ����
					DrawDebugLine(world, enemy->GetActorLocation(), player->GetActorLocation(), FColor::Blue, false, 0.2f);		// �÷��̾� Ž�� ��, �÷��̾�� Enemy�� �����ϴ� �Ķ� �� ����
					return;
				}
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGBAIController::TargetKey, nullptr);	// ĳ���� �߰� ���� ��, Target�� nullptr ����

	// Player Ž�� ���ϸ�, ���忡��, AI ���� ���� ��ǥ�� �������� 6���� �̳� 16���� ���� ���� ������ �� 0.2�ʰ� ����
	DrawDebugSphere(world, center, detectRadius, 16, FColor::Red, false, 0.2f);
}