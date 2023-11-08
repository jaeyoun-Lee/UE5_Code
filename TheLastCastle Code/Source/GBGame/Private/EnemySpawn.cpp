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

	// 보이지는 않지만, 구체 모양으로 EnemySpawn의 외형 지정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (tempMesh.Succeeded())
	{
		spawnSphere->SetStaticMesh(tempMesh.Object);
	}

	spawnSphere->bHiddenInGame = true;								// 게임 내에서 보이지 않도록 설정
	spawnSphere->SetCollisionProfileName(TEXT("NoCollision"));		// 부딪히지 않도록 콜리전을 제거

	gruxFactory = AGBEnemy::StaticClass();				// 소환될 Enemy(Grux) 클래스로 지정
	khaimeraFactory = AGBKhaimera::StaticClass();		// 소환될 Enemy(Khaimera) 클래스로 지정
	morigeshFactory = AMorigesh::StaticClass();			// 소환될 Enemy(Morigesh) 클래스로 지정
	rampageFactory = ARampage::StaticClass();			// 소환될 Enemy(Rampage) 클래스로 지정
}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();

	selectEnemy = FMath::RandRange(1, 4);		// 어떤 Enemy를 소환할지 랜덤 값 지정
	CreateEnemy();								// Enemy 스폰 함수 호출
}

// Enemy 스폰 함수
void AEnemySpawn::CreateEnemy()
{
	// BeginPlay() 함수에서 받은 랜덤 숫자로
	// 각 숫자별로 스폰되는 Enemy를 다르게함
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
