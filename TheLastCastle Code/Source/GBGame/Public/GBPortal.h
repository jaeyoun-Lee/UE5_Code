// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "GBPortal.generated.h"

UCLASS()
class GBGAME_API AGBPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBPortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 오버랩이 시작되면 몇초 머물렀는지 세는 함수
	UFUNCTION()
	void TimerStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 오버랩이 끝나면 머물렀던 초를 초기화하는 함수
	UFUNCTION()
	void TimerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 현재 머무른 초를 계산하는 함수, 2초가 넘었는지 확인하는 함수
	UFUNCTION()
	void CheckTimer();

	// 다음 스테이지로 넘어가는 함수
	UFUNCTION()
	void NextLevelOpen();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 액터의 루트 컴포넌트가 될 씬 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class USceneComponent* sceneComponent;

	// 포탈의 콜리전
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class UCapsuleComponent* collision;

	// 포탈 파티클
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UParticleSystemComponent* portal;

	// 포탈 내부 파티클
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UParticleSystemComponent* portalInside;

	// 머무르는 시간을 재는 타이머
	FTimerHandle stayTimer;

	// 다음 스테이지로 넘어가기전에 보여지는 UI
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UNextLevelWidget* nextLVWidget;

	// 다음 스테이지로 넘어가기전에 보여지는 UI의 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = UIAnim, Meta = (AllowPriavateAccess = true))
	UWidgetAnimation* UIAnimation;

	// 현재 포탈에 몇초나 머물렀는지 
	UPROPERTY()
	float currentTime;

	// UI의 애니메이션 재생 타이머
	FTimerHandle nextLevelTimer;
};
