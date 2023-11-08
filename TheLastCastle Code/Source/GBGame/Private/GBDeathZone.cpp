// Fill out your copyright notice in the Description page of Project Settings.


#include "GBDeathZone.h"
#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "GBEnemy.h"
#include "GBKhaimera.h"
#include "Morigesh.h"
#include "Rampage.h"
#include "GBGameState.h"

// Sets default values
AGBDeathZone::AGBDeathZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	deathCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Death Zone"));
	deathCollision->SetupAttachment(RootComponent);

	deathCollision->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void AGBDeathZone::BeginPlay()
{
	Super::BeginPlay();
	
	deathCollision->OnComponentBeginOverlap.AddDynamic(this, &AGBDeathZone::OverlapDeath);
}

void AGBDeathZone::OverlapDeath(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto userController = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	GBCHECK(userController != nullptr);

	if (Cast<AGBCharacter>(OtherActor))
	{
		userController->SetPause(true);
		DisableInput(userController);
		userController->ShowFailedUI();
	}

	else if (Cast<AGBEnemy>(OtherActor) || Cast<AGBKhaimera>(OtherActor) || Cast<AMorigesh>(OtherActor) || Cast<ARampage>(OtherActor))
	{
		auto gameState = Cast<AGBGameState>(UGameplayStatics::GetGameState(this));
		if (gameState != nullptr)
		{
			gameState->killedEnemy();
			OtherActor->Destroy();
		}
	}
}

// Called every frame
void AGBDeathZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

