// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalComponents/PlusButtonStrategyComponent.h"
#include "AddLeftPBSComponent.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UAddLeftPBSComponent : public UPlusButtonStrategyComponent
{
	GENERATED_BODY()

public:
	virtual void AddParts(class ABoard* Board) override;
};
