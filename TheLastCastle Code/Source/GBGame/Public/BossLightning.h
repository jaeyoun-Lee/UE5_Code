// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossLightning.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLightningAttackDelegate, class AGBCharacter*)

UCLASS()
class GBGAME_API ABossLightning : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossLightning();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 폰과 플레이어 컨트롤러가 생성되는 시점
	virtual void PostInitializeComponents() override;

	FOnLightningAttackDelegate OnLightningAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 플레이어와 콜리전이 부딪혔을 때 호출될 함수
	UFUNCTION()
	void OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 루트가 될 씬 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultSceneRoot;		

	// 콜리전 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* collision;

	// 번개 나이아가라
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* lightning;
};
