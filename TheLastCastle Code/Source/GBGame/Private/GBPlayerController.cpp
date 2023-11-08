// Fill out your copyright notice in the Description page of Project Settings.


#include "GBPlayerController.h"
#include "GBHUDWidget.h"
#include "GBStartWidget.h"
#include "NextLevelWidget.h"
#include "Boss.h"

AGBPlayerController::AGBPlayerController()
{
	static ConstructorHelpers::FClassFinder<UGBHUDWidget> playerHUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_Player.UI_Player_C'"));
	if (playerHUD.Succeeded())
	{
		HUDWidgetClass = playerHUD.Class;
	}

	static ConstructorHelpers::FClassFinder<UGBStartWidget> pauseUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_Pause.UI_Pause_C'"));
	if (pauseUI.Succeeded())
	{
		PauseWidgetClass = pauseUI.Class;
	}

	static ConstructorHelpers::FClassFinder<UGBStartWidget> faileUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_Death.UI_Death_C'"));
	if (faileUI.Succeeded())
	{
		FailedWidgetClass = faileUI.Class;
	}

	static ConstructorHelpers::FClassFinder<UGBStartWidget> clearUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_Clear.UI_Clear_C'"));
	if (clearUI.Succeeded())
	{
		ClearWidgetClass = clearUI.Class;
	}

	static ConstructorHelpers::FClassFinder<UNextLevelWidget> nextUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP/UI/UI_DarkScreen.UI_DarkScreen_C'"));
	if (nextUI.Succeeded())
	{
		NextLevelWidgetClass = nextUI.Class;
	}

	bossSpawnClass = ABoss::StaticClass();
}

// 캐릭터에게 빙의하는 시점(로그인 단계에서 실행되는 함수, BeginPlay() 함수보다 먼저 실행된다.)
void AGBPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	HUDWidget = CreateWidget<UGBHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
	ChangeInputMode(true);

	clearWidget = CreateWidget<UGBStartWidget>(this, ClearWidgetClass);
	GBCHECK(clearWidget != nullptr);

	failedWidget = CreateWidget<UGBStartWidget>(this, FailedWidgetClass);
	GBCHECK(failedWidget != nullptr);

	nextWidget = CreateWidget<UNextLevelWidget>(this, NextLevelWidgetClass);
	GBCHECK(nextWidget != nullptr);
}


void AGBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// UI 배제 후, 게임에만 입력을 전달하도록 명령
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);	
}

void AGBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AGBPlayerController::OnPause);
}
 
void AGBPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GBLOG_S(Warning);
}

UGBHUDWidget* AGBPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

// 게임 일시정지 및 Pause UI 띄우기
void AGBPlayerController::OnPause()
{
	pauseWidget = CreateWidget<UGBStartWidget>(this, PauseWidgetClass);
	GBCHECK(pauseWidget != nullptr);

	pauseWidget->AddToViewport(3);

	SetPause(true);		// 게임을 일시정지하는 함수
	ChangeInputMode(false);
}

// 게임 클리어 시, Clear UI 띄우기
void AGBPlayerController::ShowSucceededUI()
{
	clearWidget->AddToViewport();
	ChangeInputMode(false);
}

// 플레이어 사망 시, Failed UI 띄우기
void AGBPlayerController::ShowFailedUI()
{
	failedWidget->AddToViewport();
	ChangeInputMode(false);
}

void AGBPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)	// true이면, 게임 입력 모드 + 마우스 안보이도록
	{
		SetInputMode(gameInputMode);
		bShowMouseCursor = false;	
	}
	
	else         // false이면, UI 입력 모드 + 마우스 커버 보이도록
	{
		SetInputMode(uiInputMode);
		bShowMouseCursor = true;
	}
}

void AGBPlayerController::BossSpawn()
{
	if (bossSpawnClass)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			world->SpawnActor<ABoss>(bossSpawnClass, FVector(0, 0, 300), FRotator::ZeroRotator);
		}
	}
}

void AGBPlayerController::ShowNextUI()
{
	nextWidget->AddToViewport();
	ChangeInputMode(false);
}

UNextLevelWidget* AGBPlayerController::GetNextLevelWidget()
{
	return nextWidget;
}