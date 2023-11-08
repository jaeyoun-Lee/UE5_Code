// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "GBPortal.generated.h"

UCLASS()
class GBGAME_API AGBPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBPortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �������� ���۵Ǹ� ���� �ӹ������� ���� �Լ�
	UFUNCTION()
	void TimerStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// �������� ������ �ӹ����� �ʸ� �ʱ�ȭ�ϴ� �Լ�
	UFUNCTION()
	void TimerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ���� �ӹ��� �ʸ� ����ϴ� �Լ�, 2�ʰ� �Ѿ����� Ȯ���ϴ� �Լ�
	UFUNCTION()
	void CheckTimer();

	// ���� ���������� �Ѿ�� �Լ�
	UFUNCTION()
	void NextLevelOpen();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// ������ ��Ʈ ������Ʈ�� �� �� ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class USceneComponent* sceneComponent;

	// ��Ż�� �ݸ���
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class UCapsuleComponent* collision;

	// ��Ż ��ƼŬ
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UParticleSystemComponent* portal;

	// ��Ż ���� ��ƼŬ
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UParticleSystemComponent* portalInside;

	// �ӹ����� �ð��� ��� Ÿ�̸�
	FTimerHandle stayTimer;

	// ���� ���������� �Ѿ������ �������� UI
	UPROPERTY(EditDefaultsOnly, Category = Portal, Meta = (AllowPriavateAccess = true))
	class UNextLevelWidget* nextLVWidget;

	// ���� ���������� �Ѿ������ �������� UI�� �ִϸ��̼�
	UPROPERTY(EditDefaultsOnly, Category = UIAnim, Meta = (AllowPriavateAccess = true))
	UWidgetAnimation* UIAnimation;

	// ���� ��Ż�� ���ʳ� �ӹ������� 
	UPROPERTY()
	float currentTime;

	// UI�� �ִϸ��̼� ��� Ÿ�̸�
	FTimerHandle nextLevelTimer;
};
