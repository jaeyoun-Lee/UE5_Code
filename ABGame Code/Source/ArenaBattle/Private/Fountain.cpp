// Fill out your copyright notice in the Description page of Project Settings.


#include "Fountain.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFountain::AFountain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Water"));
	light = CreateDefaultSubobject<UPointLightComponent>(TEXT("light"));
	splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("splash"));
	Rot_movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

	// 스태틱 메시 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempBody(TEXT("/Script/Engine.StaticMesh'/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));

	if (tempBody.Succeeded()) {
		Body->SetStaticMesh(tempBody.Object);
	}

	// 스태틱 메시 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempWater(TEXT("/Script/Engine.StaticMesh'/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));

	if (tempWater.Succeeded()) {
		Water->SetStaticMesh(tempWater.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> splashTemplate(TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01'"));

	if (splashTemplate.Succeeded()) {
		splash->SetTemplate(splashTemplate.Object);
	}

	RootComponent = Body;
	Water->SetupAttachment(Body);
	light->SetupAttachment(Body);
	splash->SetupAttachment(Body);

	Water->SetRelativeLocation(FVector(0, 0, 135.0f));
	light->SetRelativeLocation(FVector(0, 0, 195.0f));
	splash->SetRelativeLocation(FVector(0, 0, 195.0f));

	RotatedSpeed = 30.0f;	// 생성자에서 RoatatedSpeed 초기화
	Rot_movement->RotationRate = FRotator(0, RotatedSpeed, 0);
}

// Called when the game starts or when spawned
void AFountain::BeginPlay()
{
	Super::BeginPlay();
	
	ABLOG_S(Warning);
	ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"),
									*GetName(), ID, GetActorLocation().X);
	//UE_LOG(ArenaBattle, Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
}

// Called every frame
void AFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//AddActorLocalRotation(FRotator(0, RotatedSpeed * DeltaTime, 0));
}

void AFountain::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	ABLOG_S(Warning);
}

void AFountain::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}