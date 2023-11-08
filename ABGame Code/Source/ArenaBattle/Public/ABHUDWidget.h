// Fillout your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(class UABCharacterStatComponent* CharacterStat);	// ĳ���� ���Ȱ� ���ε��� �Լ�
	void BindPlayerState(class AABPlayerState* PlayerState);				// �÷��̾� ������Ʈ ������ ���ε��� �Լ�

protected:
	virtual void NativeConstruct() override;	// UI �ý����� �غ�Ǹ� ȣ��Ǵ� �Լ�
	void UpdateCharacterStat();					// ĳ���� ���� ������Ʈ
	void UpdatePlayerState();					// �÷��̾� ������Ʈ ���� ������Ʈ

private:
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;	// ĳ���� ���� ������Ʈ�� ������ ����
	TWeakObjectPtr<class AABPlayerState> CurrentPlayerState;				// �÷��̾� ������Ʈ�� ������ ����

	UPROPERTY()
	class UProgressBar* HPBar;	// HPBar ���α׷����� ������ ����

	UPROPERTY()
	class UProgressBar* ExpBar;	// ExpBar ���α׷����� ������ ����

	UPROPERTY()
	class UTextBlock* PlayerName;	// PlayerName �ؽ�Ʈ�ڽ� ������ ����

	UPROPERTY()
	class UTextBlock* PlayerLevel;	// PlayerLevel �ؽ�Ʈ �ڽ� ������ ����

	UPROPERTY()
	class UTextBlock* CurrentScore;	// CurrentScore �ؽ�Ʈ �ڽ� ������ ����

	UPROPERTY()
	class UTextBlock* HighScore;	// HighScore �ؽ�Ʈ �ڽ� ������ ����

};
