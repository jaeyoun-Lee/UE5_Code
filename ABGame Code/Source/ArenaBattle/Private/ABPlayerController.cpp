// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGamePlayWidget.h"
#include "ABGameState.h"
#include "ABGamePlayResultWidget.h"


AABPlayerController::AABPlayerController()
{
	// 블루 프린트 Hud 위젯이므로 마지막 경로에 _C 붙이기
	static ConstructorHelpers::FClassFinder<UABHUDWidget>tempHUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HUD.UI_HUD_C'"));
	if (tempHUD.Succeeded())
	{
		// HUD위젯 클래스와 이를 상속받은 클래스들을 가져오기 위한 선언(특정 클래스와 상속받은 클래스들로 목록을 한정)
		HUDWidgetClass = tempHUD.Class;
	}

	// UI_Menu는 블루 프린트이므로 경로 마지막에 _C 추가
	static ConstructorHelpers::FClassFinder<UABGamePlayWidget> pauseUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Menu.UI_Menu_C'"));
	if (pauseUI.Succeeded())
	{
		MenuWidgetClass = pauseUI.Class;	// UABGamePlayWidget 클래스의 위젯을 모두 가져옴
	}

	// UI_Result는 블루 프린트이므로 경로 마지막에 _C 추가
	static ConstructorHelpers::FClassFinder<UABGamePlayResultWidget> resultUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Result.UI_Result_C'"));
	if (resultUI.Succeeded())
	{
		ResultWidgetClass = resultUI.Class;
	}
}

// 캐릭터에게 빙의하는 시점(로그인 단계에서 실행되는 함수, BeginPlay() 함수보다 먼저 실행된다.)
void AABPlayerController::OnPossess(APawn* aPawn) {
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);

	// BeginPlay()에 아래 코드 작성 시, 에러 발생(ABCharacter가 아직 생성되지 않은 HUDWidget을 탐색하기 때문)
	// 그러므로 BeginPlay()보다 더 빠른 시점인 OnPosses()에서 HUDWIdget 생성
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);	// 위젯 생성
	ABCHECK(HUDWidget != nullptr);
	HUDWidget->AddToViewport(1);			// 위젯 보이도록, 매개 변수 숫자가 높을수록 위젯의 우선순위가 높다.

	// 결과 UI는 굳이 OnPossess() 함수에 추가하지 않아도됨. 그대로 BeginPlay()에 있어도 된다.
	resultWidget = CreateWidget<UABGamePlayResultWidget>(this, ResultWidgetClass);	// 결과 UI 미리 만들어두기
	ABCHECK(resultWidget != nullptr);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);

	// 패키징 파일에서, BeginPlay()에 위치시키니 ABCharacter의 Begin()함수가 먼저 실행되어 오류가 발생함(HUDWIdget을 찾지 못함)
	// 그러므로 OnPosses() 함수에서 HudWidget이 생성될 수 있도록 해준다.
	//HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);	// 위젯 생성
	//ABCHECK(HUDWidget != nullptr);
	//HUDWidget->AddToViewport(1);			// 위젯 보이도록, 매개 변수 숫자가 높을수록 위젯의 우선순위가 높다.

	//resultWidget = CreateWidget<UABGamePlayResultWidget>(this, ResultWidgetClass);	// 결과 UI 미리 만들어두기
	//ABCHECK(resultWidget != nullptr);

	ABPlayerState = Cast<AABPlayerState>(PlayerState);	// 플레이어 스테이트 연결
	ABCHECK(ABPlayerState != nullptr);

	HUDWidget->BindPlayerState(ABPlayerState);				// 플레이어 스테이트 HUD 위젯과 연결
	ABPlayerState->OnPlayerStateChanged.Broadcast();		// 플레이어 스테이트가 변경됨을 알리는 델리게이트 호출
}


// 입력을 처리하기 위한 함수
void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AABPlayerController::OnGamePause);	// 게임 일시정지 함수 호출
}

// 게임 일시정지 UI 호출
void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGamePlayWidget>(this, MenuWidgetClass);	// 위젯 생성
	ABCHECK(MenuWidget != nullptr);
	MenuWidget->AddToViewport(3);	// 위젯 보이도록, 매개 변수 숫자가 높을수록 위젯의 우선순위가 높다.(HUD Widget의 ZOrder = 1)

	SetPause(true);				// 게임 일시 정지
	ChangeInputMode(false);		// UI에만 입력되도록 모드 변경
}

// NPC를 죽이면 방금 잡은 NPC의 경험치를 내 경험치로 획득
void AABPlayerController::NPCKill(AABCharacter* KiledNPC) const
{
	if (ABPlayerState->AddExp(KiledNPC->GetExp()))
		OnLevelUP.Broadcast();
}

// 게임 스코어를 1 증가시킴
void AABPlayerController::AddGameScore() const
{
	// 플레이어 스테이트에 구현된 AddGameScore 함수 호출
	ABPlayerState->AddGameScore();
}


//  게임모드 변경(UI에만 입력되도록 / 게임에만 입력되도록)
void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(gameInputMode);	// 게임에만 입력되도록
		bShowMouseCursor = false;		// 마우스 커서 안보이도록
	}

	else
	{
		SetInputMode(UIInputMode);		// UI에만 입력되도록
		bShowMouseCursor = true;		// 마우스 커서 보이도록
	}
}

// 결과 UI 띄우기 함수
void AABPlayerController::ShowResultUI()
{
	auto gameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(gameState != nullptr);

	// 결과 UI에서 게임 스테이트의 미션 클리어 여부 / 최종 점수에 대한 정보가 필요하다.
	// 그러므로, AddToViewport로 UI를 보이게하기 전에, 게임 스테이트에 대한 정보를 전달한다.
	resultWidget->BindGameState(gameState);

	resultWidget->AddToViewport();
	ChangeInputMode(false);		// UI에만 입력하도록 + 마우스 커서 보이게
}

UABHUDWidget* AABPlayerController::GetHudWidget() const
{
	return HUDWidget;
}


void AABPlayerController::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

// Pawn으로 가는 입력을 막는 코드
//void AABPlayerController::SetupInputComponent() 
//{
//	Super::SetupInputComponent();
//	InputComponent->BindAxis(TEXT("LeftRight"), this, &AABPlayerController::LeftRight);
//}
//
//void AABPlayerController::LeftRight(float NewAxisValue) 
//{
//	// Do Nothing;
//}