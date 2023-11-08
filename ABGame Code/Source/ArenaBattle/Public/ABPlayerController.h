// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLevelUPDELEGATE);
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();	// ������

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;	// �����ϴ� ����

	class UABHUDWidget* GetHudWidget() const;	// HUD���� �������� �Լ�

	void NPCKill(class AABCharacter* KilledNPC) const;	// NPC�� ���̸� ��� ���� NPC�� ����ġ�� �� ����ġ�� ȹ��

	void AddGameScore() const;		// ���� ���ھ �߰��ϴ� 

	void ChangeInputMode(bool bGameMode = true);	// ���ӿ� �ԷµǴ� ��带 �����ϴ� �Լ�(ex : UI���� �Է� ���� / ���ӿ��� �Է� ���� ��...)

	void ShowResultUI();	// ��� UI�� �����ִ� �Լ�

	FOnLevelUPDELEGATE OnLevelUP;	// ������ �� ȣ��� ��������Ʈ(ü���� Ǯ�� ä��)

protected:
	virtual void BeginPlay() override;

	// HUD���� Ŭ������ �̸� ��ӹ��� Ŭ�������� �������� ���� ����(Ư�� Ŭ������ ��ӹ��� Ŭ������� ����� ����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;	

	// �Է��� ó���ϱ� ���� �Լ�
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamePlayWidget> MenuWidgetClass;	// ���� �Ͻ����� UI ������ ���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamePlayResultWidget> ResultWidgetClass;	// ���� ��� UI ������ ���� ����

private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget;	// HUD ���� ����

	UPROPERTY()	
	class AABPlayerState* ABPlayerState;	// PlayerState ����

	UPROPERTY()
	class UABGamePlayWidget* MenuWidget;	// ���� �Ͻ����� UI ����

	UPROPERTY()
	class UABGamePlayResultWidget* resultWidget;	// ���� ��� UI ����

	FInputModeGameOnly gameInputMode;			// ���ӿ��� �Է��� �����ϵ����ϴ� ����
	FInputModeUIOnly UIInputMode;				// UI���� �Է��� �����ϵ����ϴ� ����

	// PŰ�� ������ ȣ��� �Լ�
	void OnGamePause();

// Pawn���� ���� �Է��� ���� �ڵ�
//protected:
//	virtual void SetupInputComponent() override;
//
//private:
//	void LeftRight(float NewAxisValue);
};
