// Fill out your copyright notice in the Description page of Project Settings.


#include "ABUIPlayerController.h"
#include "Blueprint/UserWidget.h"


void AABUIPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ABCHECK(UIWidgetClass != nullptr);
	
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);	// UserWidget ����
	ABCHECK(UIWidgetInstance != nullptr);

	UIWidgetInstance->AddToViewport();	// UI ���̵���

	FInputModeUIOnly Mode;	// ���ӿ��Ը� �Է��� �����ϴ� ���
	// GetCachedWidget() = ���������� ������ ������ ����������, �ʿ��� ���� ������ ���� gc �����̳ʸ� �ٽ� �������� �ʴ´�.
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());	
	SetInputMode(Mode);		// �÷��̾� ��Ʈ�ѷ����� UI�� �����ϰ� ���ӿ��Ը� �Է��� �����ϵ��� ��� ����
	bShowMouseCursor = true;	// ���콺�� ���̵��� ����
}