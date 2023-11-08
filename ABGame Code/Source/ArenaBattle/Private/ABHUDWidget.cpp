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

	// 패키징 파일에서, 위젯의 값을 포인터에 저장하고, 위젯의 델리게이트와 업데이트 함수를 바인딩하는 부분에서 null에 접근하는 오류 발생 !
	// 패키징 후, ABPlayerController보다 ABCharacter의 BeginPlay()가 먼저 실행됨. 그런데 ABCharacter의 BeginPlay()에서 ABPlayerController의 함수를 사용하므로 오류가 발생
	// ABCharacter의 BeginPlay() 함수가 실행되기 전 플레이어 컨트롤러의 BeginPlay()가 실행되게해야한다.
	CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat);	// 스탯이 변경됐을 때, 캐릭터 스탯 변경 함수 호출
}

void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
{
	ABCHECK(PlayerState != nullptr);
	CurrentPlayerState = PlayerState;
	// 플레이어 스태이트가 변경됐을 때, 플레이어 스테이트 변경 함수 호출
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 각 UI 위젯들 연결하기
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

	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());	// 체력바 비율 프로그레스바에 바인딩
}

void UABHUDWidget::UpdatePlayerState()
{
	ABCHECK(CurrentPlayerState.IsValid());

	PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));							// 캐릭터 이름으로 텍스트 블록 바인딩
	PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));		// 캐릭터 레벨로 텍스트 블록 바인딩
	ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());													// 캐릭터 경험치 비율 바인딩
	CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));			// 캐릭터 게임 점수 텍스트 블록 바인딩
	HighScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameHighScore())));		// 캐릭터 게임 최고 점수 텍스트 블록 바인딩	
}