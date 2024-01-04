// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/BoardSchemeActor.h"
#include "HydraulicLock.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydraulicLock : public ABoardSchemeActor
{
	GENERATED_BODY()

private:
	void Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void Create2Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void CreateSchemes();

public:
	AHydraulicLock();
	virtual void OnSetInputPressureAfter(FString SocketName, float Pressure) override;
};
