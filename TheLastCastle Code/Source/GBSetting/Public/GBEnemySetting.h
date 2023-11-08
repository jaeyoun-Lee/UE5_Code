// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GBEnemySetting.generated.h"

/**
 * 
 */
UCLASS(config=GBGame)
class ARENABATTLESETTING_API UGBEnemySetting : public UObject
{
	GENERATED_BODY()

public:
	UGBEnemySetting();

	// 위에서 config = GBGame으로 저장하여, GBGame 폴더내에 .ini파일을 찾는다.
	// 거기서 DefautlGBGame.ini의 상단 경로가 [/Script/ArenaBattleSetting.GBEnemySetting] 이므로, 여기에 ini 내용을 가져와준다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;		// Grux 스켈레탈 메시 경로


	// 위에서 config = GBGame으로 저장하여, GBGame 폴더내에 .ini파일을 찾는다.
	// 거기서 DefautlGBGame.ini의 상단 경로가 [/Script/ArenaBattleSetting.GBEnemySetting] 이므로, 여기에 ini 내용을 가져와준다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> KhaimeraAssets;		// Khaimera 스켈레탈 메시 경로


	// 위에서 config = GBGame으로 저장하여, GBGame 폴더내에 .ini파일을 찾는다.
	// 거기서 DefautlGBGame.ini의 상단 경로가 [/Script/ArenaBattleSetting.GBEnemySetting] 이므로, 여기에 ini 내용을 가져와준다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> MorigeshAssets;		// Morigesh 스켈레탈 메시 경로

	// 위에서 config = GBGame으로 저장하여, GBGame 폴더내에 .ini파일을 찾는다.
	// 거기서 DefautlGBGame.ini의 상단 경로가 [/Script/ArenaBattleSetting.GBEnemySetting] 이므로, 여기에 ini 내용을 가져와준다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> RampageAssets;		// Morigesh 스켈레탈 메시 경로

	// 위에서 config = GBGame으로 저장하여, GBGame 폴더내에 .ini파일을 찾는다.
	// 거기서 DefautlGBGame.ini의 상단 경로가 [/Script/ArenaBattleSetting.GBEnemySetting] 이므로, 여기에 ini 내용을 가져와준다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> SevarogAssets;		// Morigesh 스켈레탈 메시 경로
};
