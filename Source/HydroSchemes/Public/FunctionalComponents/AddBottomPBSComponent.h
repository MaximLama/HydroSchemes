// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalComponents/PlusButtonStrategyComponent.h"
#include "AddBottomPBSComponent.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UAddBottomPBSComponent : public UPlusButtonStrategyComponent
{
	GENERATED_BODY()

public:
	virtual void AddParts(class ABoard* Board) override;
};
