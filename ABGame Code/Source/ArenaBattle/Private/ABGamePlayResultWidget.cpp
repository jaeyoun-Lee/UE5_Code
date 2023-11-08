// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGamePlayResultWidget.h"
#include "ABGameState.h"
#include "Components/TextBlock.h"



void UABGamePlayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABCHECK(currentGameState.IsValid());	// 현재 게임 스테이트가 유효한지

	// 결과를 보여주는 텍스트 블록
	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	ABCHECK(Result != nullptr);	

	// 게임 스테이트에서 미션을 클리어했는지 확인하고, 게임 결과를 알려줌
	Result->SetText(FText::FromString(currentGameState->IsGameCleared() ? TEXT("Mission Complete !") : TEXT("Mission Failed...")));

	// 획득한 점수를 보여주는 텍스트 블록
	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	ABCHECK(Result != nullptr);
	
	// 게임 스테이트에서 이번 게임의 최종 점수를 알려줌
	TotalScore->SetText(FText::FromString(FString::FromInt(currentGameState->GetTotalGameScore())));
} 

// 게임 스테이트 정보를 읽어들일 수 있도록 바인딩하는 함수
void UABGamePlayResultWidget::BindGameState(AABGameState* gameState)
{
	ABCHECK(gameState != nullptr);
	currentGameState = gameState;
}