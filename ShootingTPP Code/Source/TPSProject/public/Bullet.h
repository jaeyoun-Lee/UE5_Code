// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPSPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 발사체의 이동을 담당할 컴포넌트
	// 컴포넌트의 디테일 속성은 수정 가능하되, 컴포넌트 자체를 변경하지 못하도록 VisibleAnywere 권한설정
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;


	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* collisionComp;

	// 총알 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

	// 총알 제거 함수
	void BulletDestroy();

	// 총알 속도 조절
	UPROPERTY(EditAnywhere, Category = Settings)
	float speed = 5000;

	// 액터의 특정 속성을 수정하면 호출되는 이벤트 함수
	/*virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;*/
};
