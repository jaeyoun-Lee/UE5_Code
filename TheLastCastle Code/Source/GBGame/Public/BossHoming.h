// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Actor.h"
#include "BossHoming.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHomingAttackDelegate, class AGBCharacter*)

UCLASS()
class GBGAME_API ABossHoming : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossHoming();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����

	FOnHomingAttackDelegate OnHomingAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = target, Meta = (AllowPrivateAccess = true))
	class AGBCharacter* target;

	// �Ѿ� �ܰ� ��ƼŬ
	UPROPERTY(VisibleAnywhere, Category = Homing)
	class UParticleSystemComponent* fireParticle;

	UPROPERTY(EditDefaultsOnly, Category = Homing, Meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* homingSphere;

	UPROPERTY(EditDefaultsOnly, Category = Homing, Meta = (AllowPrivateAccess = true))
	UArrowComponent* arrow;

	// ����ź ���� ȿ��
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class UParticleSystem* homingEffect;

	// ��ü SFX ȿ��
	UPROPERTY(EditAnywhere, Category = HomingEffect)
	class UAudioComponent* fireSFX;
};
