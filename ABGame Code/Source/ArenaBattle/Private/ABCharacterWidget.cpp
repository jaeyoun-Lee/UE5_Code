// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "Components/ProgressBar.h"


void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(NewCharacterStat != nullptr);

	CurrentCharacterStat = NewCharacterStat;
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

// UI������ �ʱ�ȭ�ϴ� �������� �߻�
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
			// ���α׷��� ���� �ۼ�Ʈ ����(������ ����)
			HP_Progressbar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
