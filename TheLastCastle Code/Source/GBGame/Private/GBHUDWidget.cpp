// Fill out your copyright notice in the Description page of Project Settings.


#include "GBHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UGBHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	hpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_PlayerHP")));		// UI_Player�� ���α׷����ٿ� ���ε�
	GBCHECK(hpBar != nullptr);

	hpValue = Cast<UTextBlock>(GetWidgetFromName(TEXT("TX_PlayerHP")));		// UI_Player�� �ؽ�Ʈ ��ϰ� ���ε�
	GBCHECK(hpValue != nullptr);

	enemyCnt = Cast<UTextBlock>(GetWidgetFromName(TEXT("TX_EnemyCnt")));
	GBCHECK(enemyCnt != nullptr);

	bossHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_BossHP")));
	GBCHECK(bossHP != nullptr);

	remainEnemy = Cast<UTextBlock>(GetWidgetFromName(TEXT("TX_RemainEnemy")));
	GBCHECK(remainEnemy != nullptr);

	bossImage = Cast<UImage>(GetWidgetFromName(TEXT("BossImg")));
	GBCHECK(bossImage != nullptr);


	if (GetWorld()->GetMapName().Contains("BossStage"))
	{
		enemyCnt->SetVisibility(ESlateVisibility::Hidden);
		remainEnemy->SetVisibility(ESlateVisibility::Hidden);

		bossHP->SetVisibility(ESlateVisibility::Visible);
		bossImage->SetVisibility(ESlateVisibility::Visible);
	}

	else 
	{
		enemyCnt->SetVisibility(ESlateVisibility::Visible);
		remainEnemy->SetVisibility(ESlateVisibility::Visible);

		bossHP->SetVisibility(ESlateVisibility::Hidden);
		bossImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGBHUDWidget::UpdatePlayerHP(float hpRatio)
{
	GBLOG(Warning, TEXT("UpdatePlayerHP !"));

	if (hpBar != nullptr && hpValue != nullptr)
	{
		hpBar->SetPercent(hpRatio);				// ���α׷������� �ۼ�Ʈ ����
		int32 value = FMath::Clamp<float>(FMath::RoundToFloat(hpRatio * 100.0), 0, 100);	// ���� ü�� ������ ����� �� ������ 0 ~ 100���� ����
		FString playerHP = FString::Printf(TEXT("%d"), value);							// ���� ü�� ����� ���� FString Ÿ������ ��ȯ

		GBLOG(Warning, TEXT("%s"), *playerHP);

		hpValue->SetText(FText::FromString(playerHP + " %"));		// ü�º����� ������� �ؽ�Ʈ ��� �� ����
	}

	if (hpRatio <= 0.0)
	{
		OnPlayerDie.Broadcast();			// ü���� 0���Ϸ� �������� ����ߴٴ� ��������Ʈ ȣ��
	}
}

void UGBHUDWidget::UpdateEnemyCnt(int32 eCnt)
{
	if (enemyCnt != nullptr)
	{
		FString remainEnemyCnt = FString::Printf(TEXT("%d"), eCnt);		// ���� ���� ���� FString Ÿ������ ��ȯ
		enemyCnt->SetText(FText::FromString(remainEnemyCnt));			// ���� �� UI�� ����
	}
}

void UGBHUDWidget::UpdateBossHP(float hpRatio)
{
	GBLOG(Warning, TEXT("UpdatePlayerHP !"));

	if (bossHP != nullptr)
	{
		bossHP->SetPercent(hpRatio);				// ���α׷������� �ۼ�Ʈ ����
	}

	if (hpRatio <= 0.0)
	{
		OnDie.Broadcast();
		bossHP->SetVisibility(ESlateVisibility::Hidden);
		bossImage->SetVisibility(ESlateVisibility::Hidden);
	}
}