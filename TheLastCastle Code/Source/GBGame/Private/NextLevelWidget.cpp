// Fill out your copyright notice in the Description page of Project Settings.


#include "NextLevelWidget.h"


void UNextLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

UWidgetAnimation* UNextLevelWidget::GetWidgetAnimation()
{
	return NextLevel;
}

UWidgetAnimation* UNextLevelWidget::GetShowNameAnimation()
{
	return ShowName;
}

UWidgetAnimation* UNextLevelWidget::GetShowUEAnimation()
{
	return ShowUE;
}
