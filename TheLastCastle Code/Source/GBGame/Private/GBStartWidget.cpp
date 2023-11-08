// Fill out your copyright notice in the Description page of Project Settings.


#include "GBStartWidget.h"
#include "GBPlayerController.h"
#include "Components/Button.h"
#include "GBUIPlayerController.h"
#include "Kismet/GameplayStatics.h"


void UGBStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BT_GameStart 버튼이랑 바인딩(UI_Title)
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameStart")));
	if (StartButton != nullptr)
	{
		// 버튼 클릭 시, OnStartClicked() 함수실행
		StartButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnStartClicked);
	}

	// BT_GameContinue 버튼이랑 바인딩(UI_Pause)
	ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameContinue")));
	if (ContinueButton != nullptr)
	{
		// 버튼 클릭 시, OnStartClicked() 함수실행
		ContinueButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnContinueClicked);
	}

	// BT_GameReStart 버튼이랑 바인딩(UI_Pause)
	RestartButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameReStart")));
	if (RestartButton != nullptr)
	{
		// 버튼 클릭 시, OnStartClicked() 함수실행
		RestartButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnRestartClicked);
	}

	// BT_Lobby 버튼이랑 바인딩(UI_Pause, UI_Death)
	LobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_Lobby")));
	if (LobbyButton != nullptr)
	{
		// 버튼 클릭 시, OnStartClicked() 함수실행
		LobbyButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnLobbyClicked);
	}

	// BT_Exit 버튼이랑 바인딩(UI_Title, UI_Pause, UI_Death)
	QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_Quit")));
	if (QuitButton != nullptr)
	{
		// 버튼 클릭 시, ONExitClicked() 함수실행
		QuitButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnExitClicked);
	}
}

void UGBStartWidget::OnStartClicked()
{
	// 게임 스테이지 레벨 열기
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("CastleOutSide_Modify"));
}

void UGBStartWidget::OnContinueClicked()
{
	// 진행중인 게임 다시 시작
	auto playerControl = Cast<AGBPlayerController>(GetOwningPlayer());
	GBCHECK(playerControl != nullptr);

	RemoveFromParent();							// 최상단의 UI 제거
	playerControl->ChangeInputMode(true);		// 게임 입력 모드로 변경
	playerControl->SetPause(false);				// 일시정지 해제
}

void UGBStartWidget::OnRestartClicked()
{
	// 해당 레벨 다시 시작
	auto playerControl = Cast<AGBPlayerController>(GetOwningPlayer());
	GBCHECK(playerControl != nullptr);

	playerControl->RestartLevel();
}

void UGBStartWidget::OnLobbyClicked()
{
	// 게임 타이틀 레벨로 돌아가기(= 로비로 돌아가기)
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title_Level"));
}

void UGBStartWidget::OnExitClicked()
{
	if (GetWorld()->GetMapName().Contains("Title_Level"))		// 게임 종료(Title 레벨일 경우)
	{
		auto control = Cast<AGBUIPlayerController>(GetOwningPlayer());
		if (control != nullptr)
		{
			TEnumAsByte<EQuitPreference::Type> QuitPreference = EQuitPreference::Quit;
			UKismetSystemLibrary::QuitGame(GetWorld(), control, QuitPreference, true);
		}
	}

	else   // 게임 종료(게임 스테이지일 경우)
	{
		auto control = Cast<AGBPlayerController>(GetOwningPlayer());
		if (control != nullptr)
		{
			TEnumAsByte<EQuitPreference::Type> QuitPreference = EQuitPreference::Quit;
			UKismetSystemLibrary::QuitGame(GetWorld(), control, QuitPreference, true);
		}
	}
}