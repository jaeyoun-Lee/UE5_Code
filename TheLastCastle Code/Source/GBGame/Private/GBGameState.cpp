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


// 게임 시작 시, 필드에 있는 Enemy 수를 구하는 함수
int32 AGBGameState::RemainingEnemy()
{
	enemyCount = 0;

	TArray<AActor*> allActors;		// 레벨에 있는 액터를 담을 TArray 배열

	// 현재 레벨에서, AACtor 클래스를, allAtcotrs 배열에 담음
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), allActors);	

	// 모든 액터 하나 씩 검색
	for (auto spawn : allActors)
	{
		// 액터 이름에 Enemy, Morigesh, Khaimera가 있으면 EnemyCount 1 증가
		if (spawn->GetName().Contains(TEXT("Enemy")) || spawn->GetName().Contains(TEXT("Morigesh")) || spawn->GetName().Contains(TEXT("Khaimera")) 
			|| spawn->GetName().Contains(TEXT("Rampage")) || spawn->GetName().Contains(TEXT("Boss")))
		{
			GBLOG(Warning, TEXT("%s"), *spawn->GetName());	// Enemy 이름 출력
			enemyCount++;									// Enemy 수 증가
		}
	}

	return enemyCount;
}


// Enemy가 죽으면 실행되는 함수
void AGBGameState::killedEnemy()
{
	enemyCount--;				// Enemy 수 - 1 

	if (enemyCount <= 0)		// 레벨의 모든 Enemy 제거됐다면
	{
		GBLOG(Warning, TEXT("All Enemies are dead... Stage Clear !"));
		
		// 다음 스테이지로 넘어가는 포탈 생성
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


// 현재 남은 Enemy 수 반환해주는 함수
int32 AGBGameState::GetEnemyCount()
{
	return enemyCount;
}