// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;	// TickComponent() �Լ� ��� X�̹Ƿ� false�� ����
	bWantsInitializeComponent = true;			// InitializeComponent() �Լ� ��� ��, true�� �����������.

	level = 1;
}


void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(level);
}

void UABCharacterStatComponent::SetNewLevel(int32 newLevel)
{
	auto GI = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));	// ���� �ν��Ͻ� ��������

	ABCHECK(GI != nullptr);
	currentStatData = GI->GetABChracterData(newLevel);

	if (currentStatData != nullptr)
	{
		level = newLevel;
		SetHP(currentStatData->MaxHP);	// HP Ǯ ȸ��
	}

	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), newLevel);
	}
}

// ������� �԰� ���� ü�� ����
void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(currentStatData != nullptr);
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0, currentStatData->MaxHP));	// CurrentHP - NewDamage���� ������ 0 ~ MaxHP
}

// ���� ���� ����� �� ����
float UABCharacterStatComponent::GetAttack()
{
	ABCHECK(currentStatData != nullptr, 0);
	return currentStatData->Attack;
}

// HP ����Ǹ� ����
void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();	// HP�� ���� ���� ��, ȣ��Ǵ� ��������Ʈ

	if (CurrentHP < KINDA_SMALL_NUMBER)	// KINDA_SMALL_NUMBER�� ���� ������ ������ ������ �� ���
	{
		CurrentHP = 0;
		OnHPIsZero.Broadcast();	// ü���� 0�� ������ �˸��� ��������Ʈ ȣ��
	}
}

// ����� HP�� ���� ����
float UABCharacterStatComponent::GetHPRatio()
{
	ABCHECK(currentStatData != nullptr, 0.0f);

	// ü���� 0�̸� 0.0����, �ƴϸ� �ִ� ü������ ������ ���� ü�� ���� ����
	return (currentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / currentStatData->MaxHP);
}

// NPC�� �׾��� �� �ִ� ����ġ
int32 UABCharacterStatComponent::GetDropExp() const
{
	return currentStatData->DropExp;
}

// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UABCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
