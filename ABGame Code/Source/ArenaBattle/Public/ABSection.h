// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"

UCLASS()
class ARENABATTLE_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ���Ϳ� ������Ʈ �Ӽ��� �����ϸ� �۾� ���� �������� �̸� ����� Ȯ���� �� �ְ� ���ִ� �Լ�
	virtual void OnConstruction(const FTransform& transform) override;

private:
	// ���� �ʵ�
	UPROPERTY(VisibleAnywhere, Category = Mesh, META = (AllowPrivateAccess = true))
	UStaticMeshComponent* sectionMesh;

	// ���� ��
	UPROPERTY(VisibleAnywhere, Category = Mesh, META = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> sectionDoors;

	// �ʵ� �ݸ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = trigger, META = (AllowPrivateAccess = true))
	UBoxComponent* collision;

	// ���� �������� ���� ���� �ݸ��� ������Ʈ�� �������̹Ƿ� ��� ������ TArray<UBoxComponent>
	UPROPERTY(VisibleAnywhere, Category = trigger, META = (AllowPrivateAccess = true))
	TArray<UBoxComponent*>  doorCollisions;

private:
	// ���� ����(�غ� / ���� / �Ϸ�) ����ü
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};

	void SetState(ESectionState NewState);				// ������Ʈ ���� �Լ�
	ESectionState CurrentState = ESectionState::READY;	// �ʱ� ���´� �غ� ������Ʈ

	void OperateGates(bool bOpen = true);				// ���� ���� �ݴ� �Լ�

	// �Ϸ� ������Ʈ���� ������ �� �ֵ��� ���ִ� �Ӽ�
	UPROPERTY(EditAnywhere, Category = State, META = (AllowPrivateAccess = true))
	bool bNoBattle;

	// �ʵ� �ݸ����� �ε����� ���(�غ� ���� -> ���� ����) ȣ��� �Լ�
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ���� �ݸ����� �ε����� ���(�ش� ���⿡ ���� ����) ȣ��� �Լ�
	UFUNCTION()
	void OnDoorTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// NPC ���� �Լ�
	void OnNPCSpawn();			

	// NPC�� �׾����� ȣ��� �Լ�
	UFUNCTION()
	void OnKeyNPCDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, Category = Spawn, META = (AllowPrivateAccess = true))
	float EnemySpawnTime;		// NPC ���� �ð�

	UPROPERTY(EditAnywhere, Category = Sapwn, META = (AllowPrivateAccess = true))
	float ItemBoxSpawnTime;		// ItemBox ���� �ð�

	FTimerHandle SpawnNPCTimerHandle = {};		// NPC ���� �ð��� �ٷ�� Ÿ�̸� �ڵ�
	FTimerHandle SpawnItemBoxTimeHandle = {};	// ItemBox ���� �ð��� �ٷ�� Ÿ�̸� �ڵ�
};
