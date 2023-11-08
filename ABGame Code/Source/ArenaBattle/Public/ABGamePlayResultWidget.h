// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABGamePlayWidget.h"
#include "ABGamePlayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGamePlayResultWidget : public UABGamePlayWidget
{
	GENERATED_BODY()

public:
	// �̸� UI ������ ���� ������Ʈ ������ �о���� �� �ֵ��� ���ε��� �Լ�
	void BindGameState(class AABGameState* gameState);

protected:
	virtual void NativeConstruct() override;

private:
	// ���� ���� ������Ʈ ����
	TWeakObjectPtr<class AABGameState> currentGameState;
};
