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

	// �� ���̷�Ż �޽�
	UPROPERTY(VIsibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;


	// �������۰� ����ƽ �޽� �߰�
	UPROPERTY(EditAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;


	// �÷��̾� �̵� ������Ʈ ����
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerMove;

	// �÷��̾� ���� ������Ʈ ����
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerFire;

public:

	// �Է� ���ε� ��������Ʈ
	FInputBindingDelegate onInputBindingDelegate;

	// ���� ü��
	UPROPERTY(EDitDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 hp;

	// ���� ü��
	UPROPERTY(EDitDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHP = 10;

	// �ǰ� ������ �� ó��
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// ���� ������ �� ȣ��� �Լ�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Health)
	void OnGameOver();

	// �� ��ü�� ȣ��Ǵ� �̺�Ʈ �Լ�
	// isGrenade = true�� ��ź, false�� ��������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gun)
	void OnUsingGrenade(bool isGrenade);
};
