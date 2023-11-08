// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>	// USphereComponent ����ϱ� ���ؼ� ����
#include <GameFramework/ProjectileMovementComponent.h>	// Projectile ����ϱ� ���ؼ� ����

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. �浹ü ����
	collisionComp = CreateDefaultSubobject <USphereComponent>(TEXT("Collision"));

	// 2. �浹 �������� ����(BlockAll - ��� ��ü�� �ε��� ƨ�����)
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));

	// 3. �浹ü ũ�� ����
	collisionComp->SetSphereRadius(13);

	// 4. ��Ʈ ������Ʈ�� ����
	RootComponent = collisionComp;

	// 5. �ܰ� �Ѿ� ��� ������Ʈ ����
	bodyMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Bullet body"));

	// 6. collisionComp(��Ʈ ������Ʈ)�� �ڽ� ������Ʈ�� ����
	bodyMeshComp->SetupAttachment(collisionComp);

	// 7. �浹 ��Ȱ��ȭ(�θ� �̹� �浹ü�� ������ �ֱ� ����)
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 8. �ܰ� ũ�� ����(x, y, z ��� 0.25�� �ǵ��� ����)
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// �߻�ü ������Ʈ
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	// movementComp�� ���Ž�ų ������Ʈ ����
	movementComp->SetUpdatedComponent(collisionComp);

	// �Ѿ� �ʱ�ӵ�
	movementComp->InitialSpeed = 5000;

	// �ִ� �ӵ�
	movementComp->MaxSpeed = 5000;

	// �ݵ� ����
	movementComp->bShouldBounce = true;

	// �ݵ� ��
	movementComp->Bounciness = 0.3f;

	// �Ѿ� ���� �ð� �ֱ�
	//InitialLifeSpan = 2.0f;

	// �Ѿ� ���� �Լ�

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::BulletDestroy, 2.0f, false);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::BulletDestroy()
{
	Destroy();
}

// ������ Ư�� �Ӽ��� �����ϸ� ȣ��Ǵ� �̺�Ʈ �Լ�
//void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	// speed ���� �����Ǿ����� Ȯ��
//	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
//	{
//		// ������Ÿ�� �����Ʈ ������Ʈ�� speed �� ����
//		movementComp->InitialSpeed = speed;
//		movementComp->MaxSpeed = speed;
//	}
//}