// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHoming.h"
#include "GBCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ABossHoming::ABossHoming()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	homingSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Homing_Bullet"));
	arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	fireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	fireSFX = CreateDefaultSubobject<UAudioComponent>(TEXT("FireBall Sound"));

	RootComponent = homingSphere;
	arrow->SetupAttachment(RootComponent);
	fireParticle->SetupAttachment(RootComponent);
	fireSFX->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/BP/Sphere.Sphere'"));
	if (tempMesh.Succeeded())
	{
		homingSphere->SetStaticMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> tempMat(TEXT("/Script/Engine.Material'/Game/BP/Bullet/M_FireBall.M_FireBall'"));
	if (tempMat.Succeeded())
	{
		homingSphere->SetMaterial(0, tempMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> homingParticle(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_FireHoming.P_FireHoming'"));
	if (homingParticle.Succeeded())
	{
		fireParticle->SetTemplate(homingParticle.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> bombParticle(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (bombParticle.Succeeded())
	{
		homingEffect = bombParticle.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> ballSFX(TEXT("/Script/Engine.SoundCue'/Game/StarterContent/Audio/Fire01_Cue.Fire01_Cue'"));
	if (ballSFX.Succeeded())
	{
		fireSFX->SetSound(ballSFX.Object);
	}

	homingSphere->SetCollisionProfileName(TEXT("Homing"));
	fireParticle->SetRelativeLocation(FVector(80.0f, 0, 0));
	fireParticle->SetRelativeScale3D(FVector(2.5f));

	InitialLifeSpan = 9.0f;							// 발사체 7초 후 제거
}

// Called when the game starts or when spawned
void ABossHoming::BeginPlay()
{
	Super::BeginPlay();
	
	target = Cast<AGBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	GBCHECK(target != nullptr);
}

// Called every frame
void ABossHoming::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (::IsValid(target))
	{
		FVector arrowForward = arrow->GetForwardVector() * 3.0;

		this->AddActorWorldOffset(arrowForward);

		FVector arrowLocation = GetActorLocation();
		FVector targetLocation = target->GetActorLocation();

		FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(arrowLocation, targetLocation);
		FRotator arrowRot = GetActorRotation();
		
		FRotator homingRot = FMath::RInterpTo(arrowRot, lookAtRot, DeltaTime, 2.0f);

		SetActorRotation(homingRot);
	}
}


void ABossHoming::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 플레이어가 발사체이 부딪히면 OnCharacterOverlap 함수 호출
	homingSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossHoming::OnTargetOverlap);
}

void ABossHoming::OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<AGBCharacter>(OtherActor);
	if (player != nullptr)
	{
		OnHomingAttack.Broadcast(player);

		// 위 OnHominAttack 델리게이트와 연결된 Boss::HitHoming 함수로 데미지를 준 후, 폭발 이펙트 발생
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), homingEffect, target->GetActorLocation(), GetActorRotation(), FVector(2.0f));

		// 유도탄 제거
		Destroy();
	}
}