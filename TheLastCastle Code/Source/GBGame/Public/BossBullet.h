// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossBullet.generated.h"


// �÷��̾ �߻�ü ������ �¾��� ��, ȣ��� ��������Ʈ
// DECLARE_MULTICAST_DELEGATE_OneParam�� ����Ͽ�, �߻�ü�� ���� �� Player���� �����ϴ� �Ű� ���� �߰�
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBulletAttackDelegate, class AGBCharacter*);


UCLASS()
class GBGAME_API ABossBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossBullet();
	void FireFromTo(const FVector& StartLocation, const FVector& TargetLocation);		// Boss�� ���⿡�� �÷��̾�� �Ѿ� ���󰡱�
	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//void FaceFireBall();		// FireBall�� ����ε�, ����� �ƴѰ�ó�� ���̰��ϴ� �Լ�

	// �浹 ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = Collision)
	class USphereComponent* collision;

	// �Ѿ� �ܰ� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Bullet)
	class UStaticMeshComponent* bulletComp;
	
	// �Ѿ� �ܰ� ��ƼŬ
	UPROPERTY(VisibleAnywhere, Category = Bullet)
	class UParticleSystemComponent* fireParticle;

	// �߻�ü�� �̵��� ����ϴ� ������Ʈ
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
