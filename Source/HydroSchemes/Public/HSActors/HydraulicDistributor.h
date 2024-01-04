// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/BoardSchemeActor.h"
#include "HydraulicDistributor.generated.h"

class ULeverBoxComponent;

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydraulicDistributor : public ABoardSchemeActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Lever;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ULeverBoxComponent> LeverBox;

	AHydraulicDistributor();
};
