// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossTornado.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTornadoAttackDelegate, class AGBCharacter*);		// 플레이어가 토네이도와 부딪혔을때 호출될 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTornadoEndDelegate, class AGBCharacter*);		// 플레이어가 토네이도와 떨어졌을때 호출될 델리게이트


UCLASS()
class GBGAME_API ABossTornado : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossTornado();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 폰과 플레이어 컨트롤러가 생성되는 시점
	virtual void PostInitializeComponents() override;

	// 토네이도 마크 위치에 토네이도 생성
	void StartTornado();	

	// 토네이도와 부딪혔을때 호출될 함수
	UFUNCTION()
	void OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	// 토네이도에서 떨어졌을때 호출될 함수
	UFUNCTION()
	void OnTargetEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FOnTornadoAttackDelegate OnTornadoAttack;
	FOnTornadoEndDelegate OnTornadoAvoid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	USceneComponent* sceneComp;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	class UParticleSystemComponent* tornadoMark;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	class UParticleSystemComponent* tornado;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* collision;
};
