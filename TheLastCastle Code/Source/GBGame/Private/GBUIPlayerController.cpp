// Fill out your copyright notice in the Description page of Project Settings.


#include "GBUIPlayerController.h"
#include "Blueprint/UserWidget.h"


void AGBUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GBCHECK(UIWidgetClass != nullptr);

	startWidget = CreateWidget<UUserWidget>(this, UIWidgetClass);
	GBCHECK(startWidget);

	startWidget->AddToViewport();		// 시작 UI 위젯 화면에 띄움

	FInputModeUIOnly uiMode;			// UI에만 입력되도록하는 모드 변수 생성

	// SetWidgetToFocus로 사용자 위젯 지정, 
	// GetCachedWidget() 함수로, 마지막으로 생성된 위젯을 가져옴
	uiMode.SetWidgetToFocus(startWidget->GetCachedWidget());	

	SetInputMode(uiMode);				// UI만 입력되도록 하는 모드로 설정
	bShowMouseCursor = true;			// 마우스 커서 보이도록 설정
}

