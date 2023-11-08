// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/PlayerController.h"
#include "GBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API AGBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGBPlayerController();			// ������

	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
	virtual void OnPossess(APawn* aPawn) override;		// �����ϴ� ����

	class UGBHUDWidget* GetHUDWidget() const;			// HUDWidget ��������

	void ChangeInputMode(bool bGameMode = true);		// �Է� ��� ����(���� �Է� <> UI �Է�)

	UFUNCTION(BlueprintCallable)

	void ShowSucceededUI();								// ���� Ŭ���� ��, Clear UI ����

	void ShowFailedUI();								// �÷��̾� ��� ��, Failed UI ����

	UFUNCTION(BlueprintCallable)
	void ShowNextUI();									// ���� ������ �Ѿ ��, NextWidget UI ����

	UFUNCTION(BlueprintCallable)
	class UNextLevelWidget* GetNextLevelWidget();		// ���� ������ �Ѿ�� ���� Get�ϱ�

	UFUNCTION(BlueprintCallable)						// Boss ���� �Լ�
	void BossSpawn();


protected:
	virtual void BeginPlay() override;

	// �÷��̾��� �Է��� �޾Ƶ帮�� �Լ�
	virtual void SetupInputComponent() override;

	// HUD���� Ŭ������ �̸� ��ӹ��� Ŭ�������� �������� ���� ����(Ư�� Ŭ������ ��ӹ��� Ŭ������� ����� ����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBHUDWidget> HUDWidgetClass;			

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> ClearWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> PauseWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGBStartWidget> FailedWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UNextLevelWidget> NextLevelWidgetClass;

private:
	void OnPause();			// ���� �Ͻ����� �Լ�

	UPROPERTY()
	class UGBHUDWidget* HUDWidget;		// HUD ���� ����

	UPROPERTY()
	class UGBStartWidget* clearWidget;

	UPROPERTY()
	class UGBStartWidget* pauseWidget;

	UPROPERTY()
	class UGBStartWidget* failedWidget;

	UPROPERTY()
	class UNextLevelWidget* nextWidget;

	FInputModeGameOnly gameInputMode;
	FInputModeUIOnly uiInputMode;

	UPROPERTY(VisibleAnywhere, Category = Boss, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ABoss> bossSpawnClass;
};
