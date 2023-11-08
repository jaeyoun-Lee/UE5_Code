// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBullet.h"
#include "Boss.h"
#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABossBullet::ABossBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌 컴포넌트 생성
	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	bulletComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet"));
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	fireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/BP/Sphere_Modify.Sphere_Modify'"));
	if (tempMesh.Succeeded())
	{
		bulletComp->SetStaticMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> bulletMat(TEXT("/Script/Engine.Material'/Game/BP/Bullet/M_FireBall.M_FireBall'"));
	if (tempMesh.Succeeded())
	{
		bulletComp->SetMaterial(0, bulletMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempParticle(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_FireHoming.P_FireHoming'"));
	if (tempParticle.Succeeded())
	{
		fireParticle->SetTemplate(tempParticle.Object);
	}

	RootComponent = collision;						// 루트 컴포넌트로 등록
	bulletComp->SetupAttachment(RootComponent);			// 투사체 외관을 콜리전의 자식 컴포넌트로 지정
	movementComp->SetUpdatedComponent(RootComponent);	// movement 컴포넌트가 갱신시킬 컴포넌트 지정(충돌체가 있어야 충돌효과를 발생시킬 수 있으므로)
	fireParticle->SetupAttachment(RootComponent);

	collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));		// 충돌 프로파일 OverlapAllDynamic로 모든 물체와 오버랩
	collision->SetSphereRadius(77);										// 구체 반지름 13cm
	bulletComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bulletComp->SetRelativeLocationAndRotation(FVector(22, -64, -24.0f), FRotator(70, -70, 0));
	bulletComp->SetRelativeScale3D(FVector(1.5f));
	fireParticle->SetRelativeLocation(FVector(175.0f, 0, 0));
	fireParticle->SetRelativeScale3D(FVector(2.5f));
	movementComp->InitialSpeed = 2000;				// 발사체 초기 속도
	movementComp->MaxSpeed = 2000;					// 발사체 최대 속도
	movementComp->bRotationFollowsVelocity = true;	// 발사체 속도 방향과 일치하도록 매 프레임마다 회전이 업데이트
	movementComp->bShouldBounce = false;			// 발사체 반동 여부

	InitialLifeSpan = 2.0f;							// 발사체 2초 후 제거
}

// Boss의 무기에서 플레이어에게 총알 날라가기
void ABossBullet::FireFromTo(const FVector& StartLocation, const FVector& TargetLocation)
{
	// Calculate the direction from StartLocation to TargetLocation
	FVector ShootDirection = (TargetLocation - StartLocation).GetSafeNormal();

	// Set the projectile's initial velocity
	movementComp->Velocity = ShootDirection * movementComp->InitialSpeed;
}


void ABossBullet::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	collision->OnComponentBeginOverlap.AddDynamic(this, &ABossBullet::OnCharacterOverlap);	// 플레이어가 발사체이 부딪히면 OnCharacterOverlap 함수 호출
}


// 플레이어가 발사체이 부딪히면 호출되는 함수
// 발사체에 맞은게 Player이면, 해당 player를 매개 변수로 전달하며 델리게이트 호출
void ABossBullet::OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
									int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<AGBCharacter>(OtherActor);
	if (player != nullptr)
	{
		OnBulletAttack.Broadcast(player);
	}
}

// Called when the game starts or when spawned
void ABossBullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABossBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

