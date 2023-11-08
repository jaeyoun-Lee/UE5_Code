// Fill out your copyright notice in the Description page of Project Settings.


#include "ABUIPlayerController.h"
#include "Blueprint/UserWidget.h"


void AABUIPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ABCHECK(UIWidgetClass != nullptr);
	
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);	// UserWidget 지정
	ABCHECK(UIWidgetInstance != nullptr);

	UIWidgetInstance->AddToViewport();	// UI 보이도록

	FInputModeUIOnly Mode;	// 게임에게만 입력을 전달하는 모드
	// GetCachedWidget() = 마지막으로 생성된 위젯을 가져오지만, 필요한 경위 우젯에 대한 gc 컨테이너를 다시 생성하지 않는다.
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());	
	SetInputMode(Mode);		// 플레이어 컨트롤러에게 UI를 배제하고 게임에게만 입력을 전달하도록 모드 설정
	bShowMouseCursor = true;	// 마우스는 보이도록 설정
}