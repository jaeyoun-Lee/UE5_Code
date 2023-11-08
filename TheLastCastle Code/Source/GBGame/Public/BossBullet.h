// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossBullet.generated.h"


// 플레이어가 발사체 공격을 맞았을 때, 호출될 델리게이트
// DECLARE_MULTICAST_DELEGATE_OneParam를 사용하여, 발사체에 맞은 게 Player임을 전달하는 매개 변수 추가
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBulletAttackDelegate, class AGBCharacter*);


UCLASS()
class GBGAME_API ABossBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossBullet();
	void FireFromTo(const FVector& StartLocation, const FVector& TargetLocation);		// Boss의 무기에서 플레이어에게 총알 날라가기
	virtual void PostInitializeComponents() override;	// 폰과 플레이어 컨트롤러가 생성되는 시점

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//void FaceFireBall();		// FireBall이 평면인데, 평면이 아닌것처럼 보이게하는 함수

	// 충돌 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Collision)
	class USphereComponent* collision;

	// 총알 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Bullet)
	class UStaticMeshComponent* bulletComp;
	
	// 총알 외관 파티클
	UPROPERTY(VisibleAnywhere, Category = Bullet)
	class UParticleSystemComponent* fireParticle;

	// 발사체의 이동을 담당하는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = movement)
	class UProjectileMovementComponent* movementComp;

	FOnBulletAttackDelegate OnBulletAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
