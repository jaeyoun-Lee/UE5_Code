// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPSPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �߻�ü�� �̵��� ����� ������Ʈ
	// ������Ʈ�� ������ �Ӽ��� ���� �����ϵ�, ������Ʈ ��ü�� �������� ���ϵ��� VisibleAnywere ���Ѽ���
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;


	// �浹 ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* collisionComp;

	// �Ѿ� �ܰ� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

	// �Ѿ� ���� �Լ�
	void BulletDestroy();

	// �Ѿ� �ӵ� ����
	UPROPERTY(EditAnywhere, Category = Settings)
	float speed = 5000;

	// ������ Ư�� �Ӽ��� �����ϸ� ȣ��Ǵ� �̺�Ʈ �Լ�
	/*virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;*/
};
