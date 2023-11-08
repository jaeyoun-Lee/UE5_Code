// Fill out your copyright notice in the Description page of Project Settings.


#include "GBGameMode.h"
#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "GBGameState.h"

AGBGameMode::AGBGameMode()
{
	DefaultPawnClass = AGBCharacter::StaticClass();
	PlayerControllerClass = AGBPlayerController::StaticClass();
	GameStateClass = AGBGameState::StaticClass();
}

