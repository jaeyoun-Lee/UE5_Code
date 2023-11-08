// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick�Լ��� ������� �ʱ� ������ Tick() �Լ��� �� ������ ȣ����� �ʵ��� ����
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. ���� ���� �ð� ���ϱ�
	float createTime = FMath::RandRange(minTime, maxTime);

	//2. TimerManager���� �˶����
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);

	// ������ġ ���� �Ҵ�
	FindSpawnPoints();
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// �� ���� �Լ� + �˶��� ���� �ð��� �ٽ� �︮���� Ÿ�̸� ����
void AEnemyManager::CreateEnemy()
{
	// ���� ��ġ ���ϱ�(spawnPoints�迭���� �������� �� ��ǥ �ϳ��� �̾Ƽ� �� ��ġ�� ��ȯ)
	int index = FMath::RandRange(0, spawnPoints.Num() - 1);

	// �� ���� �� ��ġ
	GetWorld()->SpawnActor<AEnemy>(enemyFactory, spawnPoints[index]->GetActorLocation(), FRotator(0));

	// �ٽ� ���� �ð��� CreateEnemy �Լ��� ȣ��ǵ��� Ÿ�̸Ӽ���
	float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

// ������ ��ġ Ž���ϴ� �Լ�
void AEnemyManager::FindSpawnPoints()
{
	//for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	//{
	//	AActor* spawn = *It;

	//	// ã�� ������ �̸��� �ش� ���ڿ��� �����ϰ� �ִٸ�
	//	if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
	//	{
	//		// ���� ��Ͽ��߰�
	//		spawnPoints.Add(spawn);
	//	}
	//}

	// �˻����� ã�� ����� ������ �迭
	TArray<AActor*> allActors;

	// ���ϴ� Ÿ���� ���� ��� ã��
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), allActors);

	// ã�� ����� ���� ��� �ݺ�������
	// allActors �迭�� ��� Ž��
	for (auto spawn : allActors)
	{
		// ã�� ������ �̸��� �ش� ���ڿ��� �����ϰ� �ִٸ�
		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
		{
			// ���� ��Ͽ� �߰�
			spawnPoints.Add(spawn);
		}
	}
}