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

	// 액터와 컴포넌트 속성을 설정하면 작업 중인 레벨에서 미리 결과를 확인할 수 있게 해주는 함수
	virtual void OnConstruction(const FTransform& transform) override;

private:
	// 섹션 필드
	UPROPERTY(VisibleAnywhere, Category = Mesh, META = (AllowPrivateAccess = true))
	UStaticMeshComponent* sectionMesh;

	// 섹션 문
	UPROPERTY(VisibleAnywhere, Category = Mesh, META = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> sectionDoors;

	// 필드 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = trigger, META = (AllowPrivateAccess = true))
	UBoxComponent* collision;

	// 문이 여러개면 문에 대한 콜리전 컴포넌트도 여러개이므로 묶어서 관리할 TArray<UBoxComponent>
	UPROPERTY(VisibleAnywhere, Category = trigger, META = (AllowPrivateAccess = true))
	TArray<UBoxComponent*>  doorCollisions;

private:
	// 섹션 상태(준비 / 전투 / 완료) 구조체
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};

	void SetState(ESectionState NewState);				// 스테이트 설정 함수
	ESectionState CurrentState = ESectionState::READY;	// 초기 상태는 준비 스테이트

	void OperateGates(bool bOpen = true);				// 문을 열고 닫는 함수

	// 완료 스테이트에서 시작할 수 있도록 해주는 속성
	UPROPERTY(EditAnywhere, Category = State, META = (AllowPrivateAccess = true))
	bool bNoBattle;

	// 필드 콜리전과 부딪히는 경우(준비 상태 -> 전투 상태) 호출될 함수
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 문의 콜리전과 부딪히는 경우(해당 방향에 섹션 생성) 호출될 함수
	UFUNCTION()
	void OnDoorTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// NPC 생성 함수
	void OnNPCSpawn();			

	// NPC가 죽었을때 호출될 함수
	UFUNCTION()
	void OnKeyNPCDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, Category = Spawn, META = (AllowPrivateAccess = true))
	float EnemySpawnTime;		// NPC 생성 시간

	UPROPERTY(EditAnywhere, Category = Sapwn, META = (AllowPrivateAccess = true))
	float ItemBoxSpawnTime;		// ItemBox 생성 시간

	FTimerHandle SpawnNPCTimerHandle = {};		// NPC 생성 시간을 다루는 타이머 핸들
	FTimerHandle SpawnItemBoxTimeHandle = {};	// ItemBox 생성 시간을 다루는 타이머 핸들
};
