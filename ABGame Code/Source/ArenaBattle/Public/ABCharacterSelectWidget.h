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
	void OnPrevClicked();		// 캐릭터 이전 버튼 클릭 시, 호출 되는 함수

	UFUNCTION()
	void OnNextClicked();		// 캐릭터 다음 버튼 클릭 시, 호출 되는 함수

	UFUNCTION()
	void OnConfirmClicked();	// 캐릭터 생성 버튼 클릭 시, 호출 되는 함수

protected:

	UPROPERTY()
	class UButton* PrevButton;			// 캐릭터 이전 버튼

	UPROPERTY()
	class UButton* NextButton;			// 캐릭터 다음 버튼

	UPROPERTY()
	class UEditableTextBox* TextBox;	// 캐릭터 이름 입력 텍스트 박스

	UPROPERTY()
	class UButton* ConfirmButton;	// 캐릭터 생성 확인 버튼


	UFUNCTION(BlueprintCallable)	// 블루 프린트에서 호출 가능
	void NextCharacter(bool bForward = true);	// 다음 캐릭터로 넘기는 함수(끝까지가면 처음으로 돌아오기)

	virtual void NativeConstruct() override;	// UI 시스템이 준비되면 호출되는 함수


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;		// 현재 캐릭터의 스켈레탈 메시 인덱스

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;			// 캐릭터의 스켈레탈 메시의 마지막 인덱스

	TWeakObjectPtr<USkeletalMeshComponent> targetComponent;		// 적용될 스켈레탈 메시 컴포넌트
};
