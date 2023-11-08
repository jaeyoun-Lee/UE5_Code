// Fill out your copyright notice in the Description page of Project Settings.


#include "GBHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UGBHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	hpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_PlayerHP")));		// UI_Player의 프로그레스바와 바인딩
	GBCHECK(hpBar != nullptr);

	hpValue = Cast<UTextBlock>(GetWidgetFromName(TEXT("TX_PlayerHP")));		// UI_Player의 텍스트 블록과 바인딩
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
		hpBar->SetPercent(hpRatio);				// 프로그레스바의 퍼센트 설정
		int32 value = FMath::Clamp<float>(FMath::RoundToFloat(hpRatio * 100.0), 0, 100);	// 현재 체력 비율의 백분율 값 범위를 0 ~ 100으로 설정
		FString playerHP = FString::Printf(TEXT("%d"), value);							// 현재 체력 백분율 값을 FString 타입으로 변환

		GBLOG(Warning, TEXT("%s"), *playerHP);

		hpValue->SetText(FText::FromString(playerHP + " %"));		// 체력비율의 백분율로 텍스트 블록 값 설정
	}

	if (hpRatio <= 0.0)
	{
		OnPlayerDie.Broadcast();			// 체력이 0이하로 떨어지면 사망했다는 델리게이트 호출
	}
}

void UGBHUDWidget::UpdateEnemyCnt(int32 eCnt)
{
	if (enemyCnt != nullptr)
	{
		FString remainEnemyCnt = FString::Printf(TEXT("%d"), eCnt);		// 현재 적의 수를 FString 타입으로 변환
		enemyCnt->SetText(FText::FromString(remainEnemyCnt));			// 적의 수 UI에 띄우기
	}
}

void UGBHUDWidget::UpdateBossHP(float hpRatio)
{
	GBLOG(Warning, TEXT("UpdatePlayerHP !"));

	if (bossHP != nullptr)
	{
		bossHP->SetPercent(hpRatio);				// 프로그레스바의 퍼센트 설정
	}

	if (hpRatio <= 0.0)
	{
		OnDie.Broadcast();
		bossHP->SetVisibility(ESlateVisibility::Hidden);
		bossImage->SetVisibility(ESlateVisibility::Hidden);
	}
}