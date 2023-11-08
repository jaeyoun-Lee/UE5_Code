// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);	// ü���� 0������ �� �����ϴ� ��������Ʈ ����
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);	// HP���� ����Ǹ� ȣ��Ǵ� ��������Ʈ ����

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ������ PostInitializeComponent() �Լ��� �����ϱ� ���� ����ϴ� �Լ�
	virtual void InitializeComponent() override;

	void SetNewLevel(int32 newLevel);	// ���� ����

	void SetDamage(float NewDamage);	// ������� �޴� �Լ�
	float GetAttack();					// ���� ���� ����� ����

	void SetHP(float NewHP);			// ����� HP ���� �Լ�
	float GetHPRatio();					// ���� HP�� ������ ��ȯ�ϴ� �Լ�(= ���α׷����� ����)

	int32 GetDropExp() const;		// NPC�� ����ϴ� ����ġ �� ���

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangeDelegate OnHPChanged;

private:
	struct FABCharacterData* currentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};
