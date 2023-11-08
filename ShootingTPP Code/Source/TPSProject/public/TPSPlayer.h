// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, class UInputComponent*);

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

	// 총 스켈레탈 메시
	UPROPERTY(VIsibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;


	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY(EditAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;


	// 플레이어 이동 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerMove;

	// 플레이어 공격 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerFire;

public:

	// 입력 바인딩 델리게이트
	FInputBindingDelegate onInputBindingDelegate;

	// 현재 체력
	UPROPERTY(EDitDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 hp;

	// 현재 체력
	UPROPERTY(EDitDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHP = 10;

	// 피격 당했을 때 처리
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// 게임 오버될 때 호출될 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Health)
	void OnGameOver();

	// 총 교체시 호출되는 이벤트 함수
	// isGrenade = true면 유탄, false면 스나이퍼
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gun)
	void OnUsingGrenade(bool isGrenade);
};
