// Fill out your copyright notice in the Description page of Project Settings.


#include "GBUIPlayerController.h"
#include "Blueprint/UserWidget.h"


void AGBUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GBCHECK(UIWidgetClass != nullptr);

	startWidget = CreateWidget<UUserWidget>(this, UIWidgetClass);
	GBCHECK(startWidget);

	startWidget->AddToViewport();		// ���� UI ���� ȭ�鿡 ���

	FInputModeUIOnly uiMode;			// UI���� �Էµǵ����ϴ� ��� ���� ����

	// SetWidgetToFocus�� ����� ���� ����, 
	// GetCachedWidget() �Լ���, ���������� ������ ������ ������
	uiMode.SetWidgetToFocus(startWidget->GetCachedWidget());	

	SetInputMode(uiMode);				// UI�� �Էµǵ��� �ϴ� ���� ����
	bShowMouseCursor = true;			// ���콺 Ŀ�� ���̵��� ����
}

