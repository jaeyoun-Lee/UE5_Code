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
	// ��� ����Ʈ Hud �����̹Ƿ� ������ ��ο� _C ���̱�
	static ConstructorHelpers::FClassFinder<UABHUDWidget>tempHUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HUD.UI_HUD_C'"));
	if (tempHUD.Succeeded())
	{
		// HUD���� Ŭ������ �̸� ��ӹ��� Ŭ�������� �������� ���� ����(Ư�� Ŭ������ ��ӹ��� Ŭ������� ����� ����)
		HUDWidgetClass = tempHUD.Class;
	}

	// UI_Menu�� ��� ����Ʈ�̹Ƿ� ��� �������� _C �߰�
	static ConstructorHelpers::FClassFinder<UABGamePlayWidget> pauseUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Menu.UI_Menu_C'"));
	if (pauseUI.Succeeded())
	{
		MenuWidgetClass = pauseUI.Class;	// UABGamePlayWidget Ŭ������ ������ ��� ������
	}

	// UI_Result�� ��� ����Ʈ�̹Ƿ� ��� �������� _C �߰�
	static ConstructorHelpers::FClassFinder<UABGamePlayResultWidget> resultUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Result.UI_Result_C'"));
	if (resultUI.Succeeded())
	{
		ResultWidgetClass = resultUI.Class;
	}
}

// ĳ���Ϳ��� �����ϴ� ����(�α��� �ܰ迡�� ����Ǵ� �Լ�, BeginPlay() �Լ����� ���� ����ȴ�.)
void AABPlayerController::OnPossess(APawn* aPawn) {
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);

	// BeginPlay()�� �Ʒ� �ڵ� �ۼ� ��, ���� �߻�(ABCharacter�� ���� �������� ���� HUDWidget�� Ž���ϱ� ����)
	// �׷��Ƿ� BeginPlay()���� �� ���� ������ OnPosses()���� HUDWIdget ����
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);	// ���� ����
	ABCHECK(HUDWidget != nullptr);
	HUDWidget->AddToViewport(1);			// ���� ���̵���, �Ű� ���� ���ڰ� �������� ������ �켱������ ����.

	// ��� UI�� ���� OnPossess() �Լ��� �߰����� �ʾƵ���. �״�� BeginPlay()�� �־ �ȴ�.
	resultWidget = CreateWidget<UABGamePlayResultWidget>(this, ResultWidgetClass);	// ��� UI �̸� �����α�
	ABCHECK(resultWidget != nullptr);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);

	// ��Ű¡ ���Ͽ���, BeginPlay()�� ��ġ��Ű�� ABCharacter�� Begin()�Լ��� ���� ����Ǿ� ������ �߻���(HUDWIdget�� ã�� ����)
	// �׷��Ƿ� OnPosses() �Լ����� HudWidget�� ������ �� �ֵ��� ���ش�.
	//HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);	// ���� ����
	//ABCHECK(HUDWidget != nullptr);
	//HUDWidget->AddToViewport(1);			// ���� ���̵���, �Ű� ���� ���ڰ� �������� ������ �켱������ ����.

	//resultWidget = CreateWidget<UABGamePlayResultWidget>(this, ResultWidgetClass);	// ��� UI �̸� �����α�
	//ABCHECK(resultWidget != nullptr);

	ABPlayerState = Cast<AABPlayerState>(PlayerState);	// �÷��̾� ������Ʈ ����
	ABCHECK(ABPlayerState != nullptr);

	HUDWidget->BindPlayerState(ABPlayerState);				// �÷��̾� ������Ʈ HUD ������ ����
	ABPlayerState->OnPlayerStateChanged.Broadcast();		// �÷��̾� ������Ʈ�� ������� �˸��� ��������Ʈ ȣ��
}


// �Է��� ó���ϱ� ���� �Լ�
void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AABPlayerController::OnGamePause);	// ���� �Ͻ����� �Լ� ȣ��
}

// ���� �Ͻ����� UI ȣ��
void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGamePlayWidget>(this, MenuWidgetClass);	// ���� ����
	ABCHECK(MenuWidget != nullptr);
	MenuWidget->AddToViewport(3);	// ���� ���̵���, �Ű� ���� ���ڰ� �������� ������ �켱������ ����.(HUD Widget�� ZOrder = 1)

	SetPause(true);				// ���� �Ͻ� ����
	ChangeInputMode(false);		// UI���� �Էµǵ��� ��� ����
}

// NPC�� ���̸� ��� ���� NPC�� ����ġ�� �� ����ġ�� ȹ��
void AABPlayerController::NPCKill(AABCharacter* KiledNPC) const
{
	if (ABPlayerState->AddExp(KiledNPC->GetExp()))
		OnLevelUP.Broadcast();
}

// ���� ���ھ 1 ������Ŵ
void AABPlayerController::AddGameScore() const
{
	// �÷��̾� ������Ʈ�� ������ AddGameScore �Լ� ȣ��
	ABPlayerState->AddGameScore();
}


//  ���Ӹ�� ����(UI���� �Էµǵ��� / ���ӿ��� �Էµǵ���)
void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(gameInputMode);	// ���ӿ��� �Էµǵ���
		bShowMouseCursor = false;		// ���콺 Ŀ�� �Ⱥ��̵���
	}

	else
	{
		SetInputMode(UIInputMode);		// UI���� �Էµǵ���
		bShowMouseCursor = true;		// ���콺 Ŀ�� ���̵���
	}
}

// ��� UI ���� �Լ�
void AABPlayerController::ShowResultUI()
{
	auto gameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(gameState != nullptr);

	// ��� UI���� ���� ������Ʈ�� �̼� Ŭ���� ���� / ���� ������ ���� ������ �ʿ��ϴ�.
	// �׷��Ƿ�, AddToViewport�� UI�� ���̰��ϱ� ����, ���� ������Ʈ�� ���� ������ �����Ѵ�.
	resultWidget->BindGameState(gameState);

	resultWidget->AddToViewport();
	ChangeInputMode(false);		// UI���� �Է��ϵ��� + ���콺 Ŀ�� ���̰�
}

UABHUDWidget* AABPlayerController::GetHudWidget() const
{
	return HUDWidget;
}


void AABPlayerController::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

// Pawn���� ���� �Է��� ���� �ڵ�
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