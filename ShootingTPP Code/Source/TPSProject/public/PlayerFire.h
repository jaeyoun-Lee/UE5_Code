// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerFire.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(PlayerComponent), meta = (BlueprintSpawnableComponent))
class TPSPROJECT_API UPlayerFire : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UPlayerFire();

	virtual void BeginPlay() override;

	// �Է� ���ε� �Լ�
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;

public:

	// �Ѿ� ����
	UPROPERTY(EditDefaultsOnly, Category = bulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// �Ѿ� �߻� �̺�Ʈ ó�� �Լ�
	void InputFire();

	// 1�� ��ź�� ��������� ����
	bool bUsingGrenadeGun = true;

	// 1���� ���� ��ź������ ����
	void ChangeToGrenadeGun();

	// 2���� ���� �������� ������ ����
	void ChangeToSniper();

	// �������� ����
	void SniperAim();

	// �������� ���� ������ ����
	bool bSniperMode = false;

	// �������� UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category = sniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	// �������� UI ���� �ν��Ͻ�
	class UUserWidget* sniperUI;

	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// �������� �ѿ��� ���� ��带 �����ص� �Ϲ� ���� UI�� �ߵ����ϱ�
	UPROPERTY(EditDefaultsOnly, Category = sniperUI)
	TSubclassOf <class UUserWidget> crosshairUIFactory;

	// �Ϲ� ���ظ�� UI ���� �ν��Ͻ�
	class UUserWidget* crosshairUI;

	// ī�޶� ����ũ ��� ����Ʈ�� ������ ����
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// �ѻ���
	UPROPERTY(EditDefaultsOnly, Category = FireSound)
	class USoundBase* bulletSound;

	UPROPERTY()
	class UCameraComponent* tpsCamComp;

	// �� ���̷�Ż �޽�
	UPROPERTY()
	class USkeletalMeshComponent* gunMeshComp;


	// �������۰� ����ƽ �޽� �߰�
	UPROPERTY()
	class UStaticMeshComponent* sniperGunComp;
};
