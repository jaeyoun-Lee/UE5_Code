// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick함수를 사용하지 않기 때문에 Tick() 함수가 매 프레임 호출되지 않도록 설정
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. 랜덤 생성 시간 구하기
	float createTime = FMath::RandRange(minTime, maxTime);

	//2. TimerManager한테 알람등록
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);

	// 스폰위치 동적 할당
	FindSpawnPoints();
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// 적 생성 함수 + 알람이 랜덤 시간에 다시 울리도록 타이머 설정
void AEnemyManager::CreateEnemy()
{
	// 랜덤 위치 구하기(spawnPoints배열에서 랜덤으로 그 좌표 하나를 뽑아서 그 위치에 소환)
	int index = FMath::RandRange(0, spawnPoints.Num() - 1);

	// 적 생성 및 배치
	GetWorld()->SpawnActor<AEnemy>(enemyFactory, spawnPoints[index]->GetActorLocation(), FRotator(0));

	// 다시 랜덤 시간에 CreateEnemy 함수가 호출되도록 타이머설정
	float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

// 스폰할 위치 탐색하는 함수
void AEnemyManager::FindSpawnPoints()
{
	//for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	//{
	//	AActor* spawn = *It;

	//	// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
	//	if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
	//	{
	//		// 스폰 목록에추가
	//		spawnPoints.Add(spawn);
	//	}
	//}

	// 검색으로 찾은 결과를 저장할 배열
	TArray<AActor*> allActors;

	// 원하는 타입의 액터 모두 찾기
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), allActors);

	// 찾은 결과가 있을 경우 반복적으로
	// allActors 배열을 모두 탐색
	for (auto spawn : allActors)
	{
		// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
		{
			// 스폰 목록에 추가
			spawnPoints.Add(spawn);
		}
	}
}