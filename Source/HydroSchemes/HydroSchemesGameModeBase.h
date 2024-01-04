// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HydroSchemesGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydroSchemesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHydroSchemesGameModeBase();

	virtual void StartPlay() override;

};
