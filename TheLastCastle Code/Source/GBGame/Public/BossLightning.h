// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossLightning.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLightningAttackDelegate, class AGBCharacter*)

UCLASS()
class GBGAME_API ABossLightning : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossLightning();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
	virtual void PostInitializeComponents() override;

	FOnLightningAttackDelegate OnLightningAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// �÷��̾�� �ݸ����� �ε����� �� ȣ��� �Լ�
	UFUNCTION()
	void OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ��Ʈ�� �� �� ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultSceneRoot;		

	// �ݸ��� ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* collision;

	// ���� ���̾ư���
	UPROPERTY(EditDefaultsOnly, Category = Collision, Meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* lightning;
};
