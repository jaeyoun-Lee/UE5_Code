// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "Engine/StreamableManager.h"
#include "Engine/GameInstance.h"
#include "GBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GBGAME_API UGBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FStreamableManager streamableManager;
};
