// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// �÷��̾� �̵� �ӵ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float speed = 0;

	// �÷��̾� �¿� �̵� �ӵ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float direction = 0;

	// �� ������ ���ŵǴ� �Լ�
	// ��� ����Ʈ�� �ִ� Event Blueprint Update Animation ��忡 �ش��ϴ� �Լ��̴�.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// �÷��̾ ���߿� �ִ��� Ȯ���ϴ� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;


	// ����� ���� �ִϸ��̼� ��Ÿ��(�ִϸ��̼� ����� ���� ó��)
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	
	class UAnimMontage* attackAnimMontage;

	// ���� �ִϸ��̼� ��� �Լ�
	void PlayAttackAnim();
};
