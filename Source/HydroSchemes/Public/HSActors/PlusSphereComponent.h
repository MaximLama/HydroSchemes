// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "PlusSphereComponent.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UPlusSphereComponent : public USphereComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact() override;
};
