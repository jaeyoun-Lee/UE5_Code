// Fill out your copyright notice in the Description page of Project Settings.


#include "GBEnemyWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UGBEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	hpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	GBCHECK(hpBar != nullptr);

	name = Cast<UTextBlock>(GetWidgetFromName(TEXT("EnemyName")));
	GBCHECK(name != nullptr);

	UpdateHPWidget(1.0f);
	SetName(enemyText);
}


void UGBEnemyWidget::UpdateHPWidget(float hpRatio)
{
	if (hpBar != nullptr)
	{
		hpBar->SetPercent(hpRatio);
	}

	if (hpRatio <= 0.0)
	{
		OnDie.Broadcast();
	}
}

void UGBEnemyWidget::SetName(FString enemyName)
{
	name->SetText(FText::FromString(enemyName));	// 캐릭터 이름으로 텍스트 블록 바인딩
	GBLOG(Warning, TEXT("SetName Practice"));
}