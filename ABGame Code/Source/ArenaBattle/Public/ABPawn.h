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

	// 캡슐 콜리전
	UPROPERTY(EditDefaultsOnly, Category = collision)
	UCapsuleComponent* collision;
	
	// 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, Category = CardBoard)
	USkeletalMeshComponent* meshComp;

	// 무브먼트 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = moveMent)
	UFloatingPawnMovement* moveComp;

	// 스프링 암
	UPROPERTY(EditDefaultsOnly, Category = camera)
	USpringArmComponent* springArm;

	// 카메라
	UPROPERTY(EditDefaultsOnly, Category = camera)
	UCameraComponent* camComp;

private:
	// Axis 입력(축 매핑 입력 함수)
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
};
