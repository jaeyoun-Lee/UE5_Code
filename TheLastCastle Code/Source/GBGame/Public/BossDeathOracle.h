// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossDeathOracle.generated.h"

UCLASS()
class GBGAME_API ABossDeathOracle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossDeathOracle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditdefaultsOnly, Category = Particle, Meta = (AllowPrivateAccess = true))
	class UParticleSystemComponent* oracle;
};
