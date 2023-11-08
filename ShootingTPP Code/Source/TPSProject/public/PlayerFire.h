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

	// 입력 바인딩 함수
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;

public:

	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = bulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 총알 발사 이벤트 처리 함수
	void InputFire();

	// 1번 유탄총 사용중인지 여부
	bool bUsingGrenadeGun = true;

	// 1번을 눌러 유탄총으로 변경
	void ChangeToGrenadeGun();

	// 2번을 눌러 스나이퍼 총으로 변경
	void ChangeToSniper();

	// 스나이퍼 조준
	void SniperAim();

	// 스나이퍼 조준 중인지 여부
	bool bSniperMode = false;

	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = sniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	// 스나이퍼 UI 위젯 인스턴스
	class UUserWidget* sniperUI;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// 스나이퍼 총에서 조준 모드를 사용안해도 일반 조준 UI가 뜨도록하기
	UPROPERTY(EditDefaultsOnly, Category = sniperUI)
	TSubclassOf <class UUserWidget> crosshairUIFactory;

	// 일반 조준모드 UI 위젯 인스턴스
	class UUserWidget* crosshairUI;

	// 카메라 셰이크 블루 프린트를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 총사운드
	UPROPERTY(EditDefaultsOnly, Category = FireSound)
	class USoundBase* bulletSound;

	UPROPERTY()
	class UCameraComponent* tpsCamComp;

	// 총 스켈레탈 메시
	UPROPERTY()
	class USkeletalMeshComponent* gunMeshComp;


	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY()
	class UStaticMeshComponent* sniperGunComp;
};
