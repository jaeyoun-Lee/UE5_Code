// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/RotatingMovementComponent.h"
//#include "EngineMinimal.h" (이미 ArenaBattle.h에 EngineMinimal.h을 사용하고 있음)
#include "GameFramework/Actor.h"
#include "Fountain.generated.h"


UCLASS()
class ARENABATTLE_API AFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 분수의 몸체
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Body;

	// 분수의 물
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Water;

	// 분수대 조명
	UPROPERTY(EditDefaultsOnly)
	UPointLightComponent* light;

	// 물 찰랑이는 이펙트
	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* splash;

	// 정수형 멤버 변수
	UPROPERTY(EditAnywhere, Category = ID)
	int32 ID;

	// 지정한 속도로 액터를 회전시키는 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Rot_movement;

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotatedSpeed;
};
