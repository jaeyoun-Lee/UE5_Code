// Fill out your copyright notice in the Description page of Project Settings.


#include "GBStartWidget.h"
#include "GBPlayerController.h"
#include "Components/Button.h"
#include "GBUIPlayerController.h"
#include "Kismet/GameplayStatics.h"


void UGBStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BT_GameStart ��ư�̶� ���ε�(UI_Title)
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameStart")));
	if (StartButton != nullptr)
	{
		// ��ư Ŭ�� ��, OnStartClicked() �Լ�����
		StartButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnStartClicked);
	}

	// BT_GameContinue ��ư�̶� ���ε�(UI_Pause)
	ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameContinue")));
	if (ContinueButton != nullptr)
	{
		// ��ư Ŭ�� ��, OnStartClicked() �Լ�����
		ContinueButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnContinueClicked);
	}

	// BT_GameReStart ��ư�̶� ���ε�(UI_Pause)
	RestartButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_GameReStart")));
	if (RestartButton != nullptr)
	{
		// ��ư Ŭ�� ��, OnStartClicked() �Լ�����
		RestartButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnRestartClicked);
	}

	// BT_Lobby ��ư�̶� ���ε�(UI_Pause, UI_Death)
	LobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_Lobby")));
	if (LobbyButton != nullptr)
	{
		// ��ư Ŭ�� ��, OnStartClicked() �Լ�����
		LobbyButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnLobbyClicked);
	}

	// BT_Exit ��ư�̶� ���ε�(UI_Title, UI_Pause, UI_Death)
	QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_Quit")));
	if (QuitButton != nullptr)
	{
		// ��ư Ŭ�� ��, ONExitClicked() �Լ�����
		QuitButton->OnClicked.AddDynamic(this, &UGBStartWidget::OnExitClicked);
	}
}

void UGBStartWidget::OnStartClicked()
{
	// ���� �������� ���� ����
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("CastleOutSide_Modify"));
}

void UGBStartWidget::OnContinueClicked()
{
	// �������� ���� �ٽ� ����
	auto playerControl = Cast<AGBPlayerController>(GetOwningPlayer());
	GBCHECK(playerControl != nullptr);

	RemoveFromParent();							// �ֻ���� UI ����
	playerControl->ChangeInputMode(true);		// ���� �Է� ���� ����
	playerControl->SetPause(false);				// �Ͻ����� ����
}

void UGBStartWidget::OnRestartClicked()
{
	// �ش� ���� �ٽ� ����
	auto playerControl = Cast<AGBPlayerController>(GetOwningPlayer());
	GBCHECK(playerControl != nullptr);

	playerControl->RestartLevel();
}

void UGBStartWidget::OnLobbyClicked()
{
	// ���� Ÿ��Ʋ ������ ���ư���(= �κ�� ���ư���)
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title_Level"));
}

void UGBStartWidget::OnExitClicked()
{
	if (GetWorld()->GetMapName().Contains("Title_Level"))		// ���� ����(Title ������ ���)
	{
		auto control = Cast<AGBUIPlayerController>(GetOwningPlayer());
		if (control != nullptr)
		{
			TEnumAsByte<EQuitPreference::Type> QuitPreference = EQuitPreference::Quit;
			UKismetSystemLibrary::QuitGame(GetWorld(), control, QuitPreference, true);
		}
	}

	else   // ���� ����(���� ���������� ���)
	{
		auto control = Cast<AGBPlayerController>(GetOwningPlayer());
		if (control != nullptr)
		{
			TEnumAsByte<EQuitPreference::Type> QuitPreference = EQuitPreference::Quit;
			UKismetSystemLibrary::QuitGame(GetWorld(), control, QuitPreference, true);
		}
	}
}