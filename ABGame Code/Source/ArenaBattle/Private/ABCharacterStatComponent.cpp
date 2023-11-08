// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;	// TickComponent() 함수 사용 X이므로 false로 변경
	bWantsInitializeComponent = true;			// InitializeComponent() 함수 사용 시, true로 설정해줘야함.

	level = 1;
}


void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(level);
}

void UABCharacterStatComponent::SetNewLevel(int32 newLevel)
{
	auto GI = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));	// 게임 인스턴스 가져오기

	ABCHECK(GI != nullptr);
	currentStatData = GI->GetABChracterData(newLevel);

	if (currentStatData != nullptr)
	{
		level = newLevel;
		SetHP(currentStatData->MaxHP);	// HP 풀 회복
	}

	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), newLevel);
	}
}

// 대미지를 입고 남은 체력 세팅
void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(currentStatData != nullptr);
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0, currentStatData->MaxHP));	// CurrentHP - NewDamage값의 범위는 0 ~ MaxHP
}

// 피해 입힐 대미지 값 전달
float UABCharacterStatComponent::GetAttack()
{
	ABCHECK(currentStatData != nullptr, 0);
	return currentStatData->Attack;
}

// HP 변경되면 세팅
void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();	// HP가 변경 됐을 때, 호출되는 델리게이트

	if (CurrentHP < KINDA_SMALL_NUMBER)	// KINDA_SMALL_NUMBER는 무시 가능한 오차를 측정할 때 사용
	{
		CurrentHP = 0;
		OnHPIsZero.Broadcast();	// 체력이 0이 됐음을 알리는 델리게이트 호출
	}
}

// 변경된 HP의 비율 전달
float UABCharacterStatComponent::GetHPRatio()
{
	ABCHECK(currentStatData != nullptr, 0.0f);

	// 체력이 0이면 0.0전달, 아니면 최대 체력으로 나누어 남은 체력 비율 전달
	return (currentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / currentStatData->MaxHP);
}

// NPC가 죽었을 때 주는 경험치
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
