// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossTornado.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTornadoAttackDelegate, class AGBCharacter*);		// �÷��̾ ����̵��� �ε������� ȣ��� ��������Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTornadoEndDelegate, class AGBCharacter*);		// �÷��̾ ����̵��� ���������� ȣ��� ��������Ʈ


UCLASS()
class GBGAME_API ABossTornado : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossTornado();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
	virtual void PostInitializeComponents() override;

	// ����̵� ��ũ ��ġ�� ����̵� ����
	void StartTornado();	

	// ����̵��� �ε������� ȣ��� �Լ�
	UFUNCTION()
	void OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	// ����̵����� ���������� ȣ��� �Լ�
	UFUNCTION()
	void OnTargetEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FOnTornadoAttackDelegate OnTornadoAttack;
	FOnTornadoEndDelegate OnTornadoAvoid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	USceneComponent* sceneComp;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	class UParticleSystemComponent* tornadoMark;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	class UParticleSystemComponent* tornado;

	UPROPERTY(EditDefaultsOnly, Category = Tornado, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* collision;
};
