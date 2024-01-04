// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/HydraulicDistributor.h"
#include "HydraulicDistributorScheme574.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydraulicDistributorScheme574 : public AHydraulicDistributor
{
	GENERATED_BODY()

private:
	void Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void Create2Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void CreateSchemes();

public:
	AHydraulicDistributorScheme574();
	virtual void ChangeState() override;
};
