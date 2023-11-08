// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"


UABGameInstance::UABGameInstance() 
{
	// 캐릭터 레벨 별 능력치 데이터 테이블
	FString CharacterDataPath = TEXT("/Script/Engine.DataTable'/Game/Book/GameData/ABCharacterData.ABCharacterData'");

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCharacter(*CharacterDataPath);

	ABCHECK(DT_ABCharacter.Succeeded());
	ABCharacterTable = DT_ABCharacter.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
}

void UABGameInstance::Init()
{
	Super::Init();
}

FABCharacterData* UABGameInstance::GetABChracterData(int32 level)
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(level), TEXT(""));
}
