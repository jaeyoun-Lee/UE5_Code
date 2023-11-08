// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ABPawn.generated.h"

UCLASS()
class ARENABATTLE_API AABPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AABPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	// ĸ�� �ݸ���
	UPROPERTY(EditDefaultsOnly, Category = collision)
	UCapsuleComponent* collision;
	
	// ���̷�Ż �޽�
	UPROPERTY(EditDefaultsOnly, Category = CardBoard)
	USkeletalMeshComponent* meshComp;

	// �����Ʈ ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = moveMent)
	UFloatingPawnMovement* moveComp;

	// ������ ��
	UPROPERTY(EditDefaultsOnly, Category = camera)
	USpringArmComponent* springArm;

	// ī�޶�
	UPROPERTY(EditDefaultsOnly, Category = camera)
	UCameraComponent* camComp;

private:
	// Axis �Է�(�� ���� �Է� �Լ�)
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
};
