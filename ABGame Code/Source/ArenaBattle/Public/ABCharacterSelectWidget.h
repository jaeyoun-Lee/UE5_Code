// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnPrevClicked();		// ĳ���� ���� ��ư Ŭ�� ��, ȣ�� �Ǵ� �Լ�

	UFUNCTION()
	void OnNextClicked();		// ĳ���� ���� ��ư Ŭ�� ��, ȣ�� �Ǵ� �Լ�

	UFUNCTION()
	void OnConfirmClicked();	// ĳ���� ���� ��ư Ŭ�� ��, ȣ�� �Ǵ� �Լ�

protected:

	UPROPERTY()
	class UButton* PrevButton;			// ĳ���� ���� ��ư

	UPROPERTY()
	class UButton* NextButton;			// ĳ���� ���� ��ư

	UPROPERTY()
	class UEditableTextBox* TextBox;	// ĳ���� �̸� �Է� �ؽ�Ʈ �ڽ�

	UPROPERTY()
	class UButton* ConfirmButton;	// ĳ���� ���� Ȯ�� ��ư


	UFUNCTION(BlueprintCallable)	// ��� ����Ʈ���� ȣ�� ����
	void NextCharacter(bool bForward = true);	// ���� ĳ���ͷ� �ѱ�� �Լ�(���������� ó������ ���ƿ���)

	virtual void NativeConstruct() override;	// UI �ý����� �غ�Ǹ� ȣ��Ǵ� �Լ�


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;		// ���� ĳ������ ���̷�Ż �޽� �ε���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;			// ĳ������ ���̷�Ż �޽��� ������ �ε���

	TWeakObjectPtr<USkeletalMeshComponent> targetComponent;		// ����� ���̷�Ż �޽� ������Ʈ
};
