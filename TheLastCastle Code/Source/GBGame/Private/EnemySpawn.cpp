// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawn.h"
#include "GBEnemy.h"
#include "GBKhaimera.h"
#include "Morigesh.h"
#include "Rampage.h"

// Sets default values
AEnemySpawn::AEnemySpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	spawnSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Spawn"));
	spawnSphere->SetupAttachment(RootComponent);

	// �������� ������, ��ü ������� EnemySpawn�� ���� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (tempMesh.Succeeded())
	{
		spawnSphere->SetStaticMesh(tempMesh.Object);
	}

	spawnSphere->bHiddenInGame = true;								// ���� ������ ������ �ʵ��� ����
	spawnSphere->SetCollisionProfileName(TEXT("NoCollision"));		// �ε����� �ʵ��� �ݸ����� ����

	gruxFactory = AGBEnemy::StaticClass();				// ��ȯ�� Enemy(Grux) Ŭ������ ����
	khaimeraFactory = AGBKhaimera::StaticClass();		// ��ȯ�� Enemy(Khaimera) Ŭ������ ����
	morigeshFactory = AMorigesh::StaticClass();			// ��ȯ�� Enemy(Morigesh) Ŭ������ ����
	rampageFactory = ARampage::StaticClass();			// ��ȯ�� Enemy(Rampage) Ŭ������ ����
}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();

	selectEnemy = FMath::RandRange(1, 4);		// � Enemy�� ��ȯ���� ���� �� ����
	CreateEnemy();								// Enemy ���� �Լ� ȣ��
}

// Enemy ���� �Լ�
void AEnemySpawn::CreateEnemy()
{
	// BeginPlay() �Լ����� ���� ���� ���ڷ�
	// �� ���ں��� �����Ǵ� Enemy�� �ٸ�����
	switch (selectEnemy)
	{
		case 1:
			if (gruxFactory)
			{
				GetWorld()->SpawnActor<AGBEnemy>(gruxFactory, GetActorLocation(), FRotator::ZeroRotator);
			}
			break;

		case 2:
			if (khaimeraFactory)
			{
				GetWorld()->SpawnActor<AGBKhaimera>(khaimeraFactory, GetActorLocation(), FRotator::ZeroRotator);
			}
			break;

		case 3:
			if (morigeshFactory)
			{
				GetWorld()->SpawnActor<AMorigesh>(morigeshFactory, GetActorLocation(), FRotator::ZeroRotator);
			}
			break;

		case 4:
			if (rampageFactory)
			{
				GetWorld()->SpawnActor<ARampage>(rampageFactory, GetActorLocation(), FRotator::ZeroRotator);
			}
			break;
	}
}


// Called every frame
void AEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
