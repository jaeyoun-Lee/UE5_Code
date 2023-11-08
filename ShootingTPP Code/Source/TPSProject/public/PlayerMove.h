// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()
	
public:

	// ������ �߰�
	UPlayerMove();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;

	// �¿� ȸ�� �Է� ó��
	void Turn(float value);

	// ���� ȸ�� �Է� ó��
	void LookUp(float value);

public:
	// �̵��ӵ�
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float walkSpeed = 200.0f;

	// �޸��� �ӵ�
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float runSpeed = 600.0f;

	// �̵� ����
	FVector dir;

	// �¿� �̵� �Է� �̺�Ʈ ó�� �Լ�
	void InputHorizontal(float value);

	// ���� �̵� �Է� �̺�Ʈ ó�� �Լ�
	void InputVertical(float value);

	// ���� �Է� �̺�Ʈ ó�� �Լ�
	void InputJump();

	// �÷��̾� �̵� ó��
	void Move();

	// �޸��� �̺�Ʈ ó���Լ�
	void InputRun();
};
