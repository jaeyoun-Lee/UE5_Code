// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTornado.h"
#include "Particles/ParticleSystemComponent.h"
#include "GBCharacter.h"

// Sets default values
ABossTornado::ABossTornado()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	tornadoMark = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TornadoMarkFX"));
	tornado = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TornadoFX"));

	RootComponent = sceneComp;
	collision->SetupAttachment(RootComponent);
	tornadoMark->SetupAttachment(RootComponent);
	tornado->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempTornado(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_aquaStorm.P_ky_aquaStorm'"));
	if (tempTornado.Succeeded())
	{
		tornado->SetTemplate(tempTornado.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempMark(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_magicCircle1.P_ky_magicCircle1'"));
	if (tempMark.Succeeded())
	{
		tornadoMark->SetTemplate(tempMark.Object);
	}

	collision->SetRelativeLocation(FVector(-1.5f, 0, 249.0f));

	collision->SetCapsuleHalfHeight(450.0f);
	collision->SetCapsuleRadius(220.0f);

	collision->SetCollisionProfileName(TEXT("NoCollision"));

	tornado->SetRelativeScale3D(FVector(1.3f, 1.3f, 2.0f));
	tornado->SetVisibility(false);

	tornadoMark->SetRelativeScale3D(FVector(1.3f));

	InitialLifeSpan = 3.0f;		// �����ϰ�, 3�� �� ����
}

void ABossTornado::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	collision->OnComponentBeginOverlap.AddDynamic(this, &ABossTornado::OnTargetOverlap);
	collision->OnComponentEndOverlap.AddDynamic(this, &ABossTornado::OnTargetEndOverlap);
}

// ����̵� ��ũ���� ����̵��� �����ǵ��� �ϴ� �Լ�
void ABossTornado::StartTornado()
{
	GBLOG(Warning, TEXT("Start Tornado !!!"));
	collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	tornado->SetVisibility(true);
}

// ����̵��� �ε����� ȣ��� ��������Ʈ�� ���ε��� �Լ�
void ABossTornado::OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<AGBCharacter>(OtherActor);
	if (player != nullptr)
	{
		OnTornadoAttack.Broadcast(player);
	}
}

// ����̵��� �浹�� ������ ȣ��� ��������Ʈ�� ���ε��� �Լ�
void ABossTornado::OnTargetEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto player = Cast<AGBCharacter>(OtherActor);
	if (player != nullptr)
	{
		OnTornadoAvoid.Broadcast(player);
	}
}

// Called when the game starts or when spawned
void ABossTornado::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossTornado::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
