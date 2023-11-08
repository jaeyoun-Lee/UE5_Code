// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "EnemySpawn.generated.h"

UCLASS()
class GBGAME_API AEnemySpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = spawn, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* spawnSphere;

	// 스폰할 Enemy 종류
	UPROPERTY(VisibleAnywhere, Category = Enemy, Meta = (AllowPrivateAccess = true))
	int32 selectEnemy;

	// 스폰할 타입
	UPROPERTY(VisibleAnywhere, Category = Enemy, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AGBEnemy> gruxFactory;

	UPROPERTY(VisibleAnywhere, Category = Enemy, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AGBKhaimera> khaimeraFactory;

	UPROPERTY(VisibleAnywhere, Category = Enemy, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AMorigesh> morigeshFactory;

	UPROPERTY(VisibleAnywhere, Category = Enemy, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ARampage> rampageFactory;
};
