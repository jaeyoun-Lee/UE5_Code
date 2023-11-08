// Fill out your copyright notice in the Description page of Project Settings.


#include "BossDeathOracle.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABossDeathOracle::ABossDeathOracle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	oracle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Death Oracle"));
	RootComponent = oracle;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempParticle(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_magicCircle2.P_ky_magicCircle2'"));
	if (tempParticle.Succeeded())
	{
		oracle->SetTemplate(tempParticle.Object);
	}

	oracle->SetRelativeScale3D(FVector(1.7f));
}

// Called when the game starts or when spawned
void ABossDeathOracle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABossDeathOracle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

