// Fill out your copyright notice in the Description page of Project Settings.


#include "BossLightning.h"
#include "NiagaraComponent.h"
#include "GBCharacter.h"

// Sets default values
ABossLightning::ABossLightning()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	lightning = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Lightning"));

	RootComponent = DefaultSceneRoot;
	collision->SetupAttachment(RootComponent);
	lightning->SetupAttachment(collision);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/BP/FX/Nia_Lightning4.Nia_Lightning4'"));
	if (tempNiagara.Succeeded())
	{
		lightning->SetAsset(tempNiagara.Object);
	}

	collision->SetRelativeLocation(FVector(-10.6f, 0, 455.0f));
	collision->SetCapsuleHalfHeight(500.0f);
	collision->SetCapsuleRadius(23.0f);

	InitialLifeSpan = 2.1f;							// 발사체 4초 후 제거
	lightning->SetBoundsScale(10.0f);
}

void ABossLightning::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	collision->OnComponentBeginOverlap.AddDynamic(this, &ABossLightning::OnTargetOverlap);
}


void ABossLightning::OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<AGBCharacter>(OtherActor);

	if (player != nullptr)
	{
		OnLightningAttack.Broadcast(player);
	}
}


// Called when the game starts or when spawned
void ABossLightning::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossLightning::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}