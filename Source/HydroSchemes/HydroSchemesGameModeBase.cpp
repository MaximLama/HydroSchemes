// Copyright Epic Games, Inc. All Rights Reserved.


#include "HydroSchemesGameModeBase.h"
#include "Player/HUBaseCharacter.h"
#include "Player/HUPlayerController.h"

AHydroSchemesGameModeBase::AHydroSchemesGameModeBase()
{
	DefaultPawnClass = AHUBaseCharacter::StaticClass();
	PlayerControllerClass = AHUPlayerController::StaticClass();
}

void AHydroSchemesGameModeBase::StartPlay()
{
	Super::StartPlay();
}


