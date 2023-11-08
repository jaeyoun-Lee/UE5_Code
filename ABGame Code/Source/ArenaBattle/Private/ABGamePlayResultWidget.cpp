// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGamePlayResultWidget.h"
#include "ABGameState.h"
#include "Components/TextBlock.h"



void UABGamePlayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABCHECK(currentGameState.IsValid());	// ���� ���� ������Ʈ�� ��ȿ����

	// ����� �����ִ� �ؽ�Ʈ ���
	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	ABCHECK(Result != nullptr);	

	// ���� ������Ʈ���� �̼��� Ŭ�����ߴ��� Ȯ���ϰ�, ���� ����� �˷���
	Result->SetText(FText::FromString(currentGameState->IsGameCleared() ? TEXT("Mission Complete !") : TEXT("Mission Failed...")));

	// ȹ���� ������ �����ִ� �ؽ�Ʈ ���
	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	ABCHECK(Result != nullptr);
	
	// ���� ������Ʈ���� �̹� ������ ���� ������ �˷���
	TotalScore->SetText(FText::FromString(FString::FromInt(currentGameState->GetTotalGameScore())));
} 

// ���� ������Ʈ ������ �о���� �� �ֵ��� ���ε��ϴ� �Լ�
void UABGamePlayResultWidget::BindGameState(AABGameState* gameState)
{
	ABCHECK(gameState != nullptr);
	currentGameState = gameState;
}