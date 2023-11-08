// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "Components/ProgressBar.h"


void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(NewCharacterStat != nullptr);

	CurrentCharacterStat = NewCharacterStat;
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

// UI위젯을 초기화하는 시점에서 발생
void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HP_Progressbar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(HP_Progressbar != nullptr);

	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (HP_Progressbar != nullptr)
		{
			// 프로그레스 바의 퍼센트 설정(게이지 설정)
			HP_Progressbar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
