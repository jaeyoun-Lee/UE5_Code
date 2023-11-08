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

	// �浹 ������Ʈ ����
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

	RootComponent = collision;						// ��Ʈ ������Ʈ�� ���
	bulletComp->SetupAttachment(RootComponent);			// ����ü �ܰ��� �ݸ����� �ڽ� ������Ʈ�� ����
	movementComp->SetUpdatedComponent(RootComponent);	// movement ������Ʈ�� ���Ž�ų ������Ʈ ����(�浹ü�� �־�� �浹ȿ���� �߻���ų �� �����Ƿ�)
	fireParticle->SetupAttachment(RootComponent);

	collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));		// �浹 �������� OverlapAllDynamic�� ��� ��ü�� ������
	collision->SetSphereRadius(77);										// ��ü ������ 13cm
	bulletComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bulletComp->SetRelativeLocationAndRotation(FVector(22, -64, -24.0f), FRotator(70, -70, 0));
	bulletComp->SetRelativeScale3D(FVector(1.5f));
	fireParticle->SetRelativeLocation(FVector(175.0f, 0, 0));
	fireParticle->SetRelativeScale3D(FVector(2.5f));
	movementComp->InitialSpeed = 2000;				// �߻�ü �ʱ� �ӵ�
	movementComp->MaxSpeed = 2000;					// �߻�ü �ִ� �ӵ�
	movementComp->bRotationFollowsVelocity = true;	// �߻�ü �ӵ� ����� ��ġ�ϵ��� �� �����Ӹ��� ȸ���� ������Ʈ
	movementComp->bShouldBounce = false;			// �߻�ü �ݵ� ����

	InitialLifeSpan = 2.0f;							// �߻�ü 2�� �� ����
}

// Boss�� ���⿡�� �÷��̾�� �Ѿ� ���󰡱�
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

	collision->OnComponentBeginOverlap.AddDynamic(this, &ABossBullet::OnCharacterOverlap);	// �÷��̾ �߻�ü�� �ε����� OnCharacterOverlap �Լ� ȣ��
}


// �÷��̾ �߻�ü�� �ε����� ȣ��Ǵ� �Լ�
// �߻�ü�� ������ Player�̸�, �ش� player�� �Ű� ������ �����ϸ� ��������Ʈ ȣ��
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

