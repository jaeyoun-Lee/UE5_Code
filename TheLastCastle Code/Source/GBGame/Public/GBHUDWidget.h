// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Blueprint/UserWidget.h"
#include "GBHUDWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerDie);
DECLARE_MULTICAST_DELEGATE(FOnBossDie);

/**
 * 
 */
UCLASS()
class GBGAME_API UGBHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdatePlayerHP(float hpRatio);		// ü���� ����Ǹ� ������ �Լ�(���α׷����ٿ� �ؽ�Ʈ ��� ����)
	void UpdateEnemyCnt(int32 eCnt);		// ü���� ����Ǹ� ������ �Լ�(���α׷����ٿ� �ؽ�Ʈ ��� ����)

	void UpdateBossHP(float hpRatio);		// ������ ü���� ����Ǹ� ������ �Լ�(���α׷����ٿ� �ؽ�Ʈ ��� ����)

	FOnPlayerDie OnPlayerDie;				// �÷��̾� ü���� 0���ϰ��Ǹ� ȣ��� ��������Ʈ
	FOnBossDie OnDie;

protected:
	// UI �ʱ�ȭ �Ǵ� ������ ȣ��Ǵ� �Լ�
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UProgressBar* hpBar;			// ü�� ProgreesBar

	UPROPERTY()
	class UTextBlock* hpValue;			// ü�� �ۼ�Ʈ�� ������ �ؽ�Ʈ ���

	UPROPERTY()
	class UTextBlock* enemyCnt;			// ���� Enemy ���� ������ �ؽ�Ʈ ���

	UPROPERTY()
	class UProgressBar* bossHP;			// ���� ü�� ProgreesBar

	UPROPERTY()
	class UTextBlock* remainEnemy;		// Remaining Enemy �ؽ�Ʈ ��� 

	UPROPERTY()
	class UImage* bossImage;		// Remaining Enemy �ؽ�Ʈ ��� 
};
