// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"


UABGameInstance::UABGameInstance() 
{
	// ĳ���� ���� �� �ɷ�ġ ������ ���̺�
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
