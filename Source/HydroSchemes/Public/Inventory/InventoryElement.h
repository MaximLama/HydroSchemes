// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryElement.generated.h"

class ASchemeActor;

/**
 * 
 */
USTRUCT(BlueprintType)
struct HYDROSCHEMES_API FInventoryElement
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ElementName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASchemeActor> ElementClass;
};
