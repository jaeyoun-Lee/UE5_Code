// Fill out your copyright notice in the Description page of Project Settings.


#include "GBGameState.h"
#include "GBPlayerController.h"
#include "GBHUDWidget.h"
#include "GBPortal.h"


AGBGameState::AGBGameState()
{
	enemyCount = RemainingEnemy();
	GBLOG(Warning, TEXT("Numbers of Enemy = %d"), GetEnemyCount());

	stage1_Location = FVector(2268.92f, 176.9626f, 8504.743f);
	stage2_Location = FVector(7801.335f, -340.234f, -500.0f);
}


// ���� ���� ��, �ʵ忡 �ִ� Enemy ���� ���ϴ� �Լ�
int32 AGBGameState::RemainingEnemy()
{
	enemyCount = 0;

	TArray<AActor*> allActors;		// ������ �ִ� ���͸� ���� TArray �迭

	// ���� ��������, AACtor Ŭ������, allAtcotrs �迭�� ����
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), allActors);	

	// ��� ���� �ϳ� �� �˻�
	for (auto spawn : allActors)
	{
		// ���� �̸��� Enemy, Morigesh, Khaimera�� ������ EnemyCount 1 ����
		if (spawn->GetName().Contains(TEXT("Enemy")) || spawn->GetName().Contains(TEXT("Morigesh")) || spawn->GetName().Contains(TEXT("Khaimera")) 
			|| spawn->GetName().Contains(TEXT("Rampage")) || spawn->GetName().Contains(TEXT("Boss")))
		{
			GBLOG(Warning, TEXT("%s"), *spawn->GetName());	// Enemy �̸� ���
			enemyCount++;									// Enemy �� ����
		}
	}

	return enemyCount;
}


// Enemy�� ������ ����Ǵ� �Լ�
void AGBGameState::killedEnemy()
{
	enemyCount--;				// Enemy �� - 1 

	if (enemyCount <= 0)		// ������ ��� Enemy ���ŵƴٸ�
	{
		GBLOG(Warning, TEXT("All Enemies are dead... Stage Clear !"));
		
		// ���� ���������� �Ѿ�� ��Ż ����
		UWorld* world = GetWorld();
		if (world != nullptr)
		{
			FActorSpawnParameters SpawnParams;

			if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleOutSide")))
			{
				auto portal = world->SpawnActor<AGBPortal>(AGBPortal::StaticClass(), stage1_Location, FRotator::ZeroRotator, SpawnParams);
			}

			else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleInSide")))
			{
				auto portal = world->SpawnActor<AGBPortal>(AGBPortal::StaticClass(), stage2_Location, FRotator::ZeroRotator, SpawnParams);
			}

			else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("Boss")))
			{
				OnGameClear.Broadcast();
			}
		}
	}

	auto userController = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (userController)
	{
		userController->GetHUDWidget()->UpdateEnemyCnt(GetEnemyCount());
	}
}


// ���� ���� Enemy �� ��ȯ���ִ� �Լ�
int32 AGBGameState::GetEnemyCount()
{
	return enemyCount;
}