// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 액터가 생성되는 시점에 호출되는 함수 + OnComponentBeginOverlap을 여기서 호출
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 플레이어를 감지하는 콜리전 박스
	UPROPERTY(VisibleAnywhere, Category = Colliison)
	UBoxComponent* trigger;

	// 아이템 상자 스태틱 메시
	UPROPERTY(VisibleAnywhere, Category = ItemBox)
	UStaticMeshComponent* ItemBox;

	// 무기인 ABWeapon 클래스 오브젝트를 가져오기 위한 선언
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;

	// 아이템 상자 습득 시, 재생될 이펙트
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* effect;

private:

	// 오버랩될 때, 실행할 함수
	UFUNCTION()
	void OnChracterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 파티클이 종료되는 시점을 알려주는 OnSystemFinished 델리게이트가 호출할 함수
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);
};
