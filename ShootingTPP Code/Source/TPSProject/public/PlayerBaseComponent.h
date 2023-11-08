// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 사용자 입력 맵핑 처리함수(앞에 virtual은 오버라이딩할 수 있는 키워드)
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) {};

	// BeginPlay() 보다 먼저 호출되는 라이프 사이클 함수
	virtual void InitializeComponent() override;

	// 컴포넌트 소유 액터
	UPROPERTY()
	ATPSPlayer* me;

	// 캐릭터 이동 담당
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
};
