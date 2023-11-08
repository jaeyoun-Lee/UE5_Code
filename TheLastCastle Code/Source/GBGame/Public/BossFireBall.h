// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossFireBall.generated.h"

UCLASS()
class GBGAME_API ABossFireBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossFireBall();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FaceFireBall();

	// 충돌 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Collision)
	class USphereComponent* collision;

	// 충돌 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Plane)
	class UStaticMeshComponent* plane;

	UPROPERTY(EditDefaultsOnly, Category = PointLight)
	class UPointLightComponent* pointLight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimerHandle RepeatTimerHandle;		// 타이머
	float repeatInterval;				// 시간 간격
};
