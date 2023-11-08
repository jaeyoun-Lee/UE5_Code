// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ABCharacterStatComponent.h"
#include "ABPlayerState.h"


void UABHUDWidget::BindCharacterStat(UABCharacterStatComponent* CharacterStat)
{
	ABCHECK(CharacterStat != nullptr);
	CurrentCharacterStat = CharacterStat;

	// ��Ű¡ ���Ͽ���, ������ ���� �����Ϳ� �����ϰ�, ������ ��������Ʈ�� ������Ʈ �Լ��� ���ε��ϴ� �κп��� null�� �����ϴ� ���� �߻� !
	// ��Ű¡ ��, ABPlayerController���� ABCharacter�� BeginPlay()�� ���� �����. �׷��� ABCharacter�� BeginPlay()���� ABPlayerController�� �Լ��� ����ϹǷ� ������ �߻�
	// ABCharacter�� BeginPlay() �Լ��� ����Ǳ� �� �÷��̾� ��Ʈ�ѷ��� BeginPlay()�� ����ǰ��ؾ��Ѵ�.
	CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat);	// ������ ������� ��, ĳ���� ���� ���� �Լ� ȣ��
}

void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
{
	ABCHECK(PlayerState != nullptr);
	CurrentPlayerState = PlayerState;
	// �÷��̾� ������Ʈ�� ������� ��, �÷��̾� ������Ʈ ���� �Լ� ȣ��
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// �� UI ������ �����ϱ�
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));	
	ABCHECK(HPBar != nullptr);

	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	ABCHECK(ExpBar != nullptr);

	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	ABCHECK(PlayerName != nullptr);

	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	ABCHECK(PlayerLevel != nullptr);

	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
	ABCHECK(CurrentScore != nullptr);

	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
	ABCHECK(HighScore != nullptr);
}

void UABHUDWidget::UpdateCharacterStat()
{
	ABCHECK(CurrentCharacterStat.IsValid());

	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());	// ü�¹� ���� ���α׷����ٿ� ���ε�
}

void UABHUDWidget::UpdatePlayerState()
{
	ABCHECK(CurrentPlayerState.IsValid());

	PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));							// ĳ���� �̸����� �ؽ�Ʈ ��� ���ε�
	PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));		// ĳ���� ������ �ؽ�Ʈ ��� ���ε�
	ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());													// ĳ���� ����ġ ���� ���ε�
	CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));			// ĳ���� ���� ���� �ؽ�Ʈ ��� ���ε�
	HighScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameHighScore())));		// ĳ���� ���� �ְ� ���� �ؽ�Ʈ ��� ���ε�	
}