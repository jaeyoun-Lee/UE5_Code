// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABCharacterStatComponent.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ĳ���� ���Ȱ� UI�� �����ϴ� �Լ�
	void BindCharacterStat(UABCharacterStatComponent* NewCharacterStat);

protected:
	// UI ������ �ʱ�ȭ�ϴ� �������� �߻��ϴ� �Լ�
	virtual void NativeConstruct() override;

	// ���α׷��� ���� �ۼ�Ʈ�� �����ϴ� �Լ�
	void UpdateHPWidget();

private:
	// ĳ���� ���� ������Ʈ�� ����
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;

	UPROPERTY()
	class UProgressBar* HP_Progressbar;	// ���α׷�����
};
