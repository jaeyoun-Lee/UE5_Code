// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/RotatingMovementComponent.h"
//#include "EngineMinimal.h" (�̹� ArenaBattle.h�� EngineMinimal.h�� ����ϰ� ����)
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

	// �м��� ��ü
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Body;

	// �м��� ��
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Water;

	// �м��� ����
	UPROPERTY(EditDefaultsOnly)
	UPointLightComponent* light;

	// �� �����̴� ����Ʈ
	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* splash;

	// ������ ��� ����
	UPROPERTY(EditAnywhere, Category = ID)
	int32 ID;

	// ������ �ӵ��� ���͸� ȸ����Ű�� �����Ʈ ������Ʈ
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Rot_movement;

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotatedSpeed;
};
