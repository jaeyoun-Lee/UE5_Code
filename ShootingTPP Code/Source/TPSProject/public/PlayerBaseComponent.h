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

	// ����� �Է� ���� ó���Լ�(�տ� virtual�� �������̵��� �� �ִ� Ű����)
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) {};

	// BeginPlay() ���� ���� ȣ��Ǵ� ������ ����Ŭ �Լ�
	virtual void InitializeComponent() override;

	// ������Ʈ ���� ����
	UPROPERTY()
	ATPSPlayer* me;

	// ĳ���� �̵� ���
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
};
