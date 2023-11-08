// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>	// USphereComponent 사용하기 위해서 선언
#include <GameFramework/ProjectileMovementComponent.h>	// Projectile 사용하기 위해서 선언

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 충돌체 생성
	collisionComp = CreateDefaultSubobject <USphereComponent>(TEXT("Collision"));

	// 2. 충돌 프로파일 설정(BlockAll - 모든 물체와 부딪혀 튕기게함)
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));

	// 3. 충돌체 크기 설정
	collisionComp->SetSphereRadius(13);

	// 4. 루트 컴포넌트로 변경
	RootComponent = collisionComp;

	// 5. 외관 총알 모양 컴포넌트 생성
	bodyMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Bullet body"));

	// 6. collisionComp(루트 컴포넌트)의 자식 컴포넌트로 지정
	bodyMeshComp->SetupAttachment(collisionComp);

	// 7. 충돌 비활성화(부모가 이미 충돌체를 가지고 있기 때문)
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 8. 외관 크기 설정(x, y, z 모두 0.25가 되도록 설정)
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// 발사체 컴포넌트
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	// movementComp가 갱신시킬 컴포넌트 지정
	movementComp->SetUpdatedComponent(collisionComp);

	// 총알 초기속도
	movementComp->InitialSpeed = 5000;

	// 최대 속도
	movementComp->MaxSpeed = 5000;

	// 반동 여부
	movementComp->bShouldBounce = true;

	// 반동 값
	movementComp->Bounciness = 0.3f;

	// 총알 생명 시간 주기
	//InitialLifeSpan = 2.0f;

	// 총알 제거 함수

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

// 액터의 특정 속성을 수정하면 호출되는 이벤트 함수
//void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	// speed 값이 수정되었는지 확인
//	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
//	{
//		// 프로젝타일 무브먼트 컴포넌트에 speed 값 적용
//		movementComp->InitialSpeed = speed;
//		movementComp->MaxSpeed = speed;
//	}
//}