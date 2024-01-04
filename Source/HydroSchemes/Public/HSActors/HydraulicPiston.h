// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/BoardSchemeActor.h"
#include "HydraulicPiston.generated.h"

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydraulicPiston : public ABoardSchemeActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> PistonRod;

	void Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void CreateSchemes();
	bool FindTank(FString SocketName);
	void UpdateState();
	virtual void OnSetInputPressureAfter(FString SocketName, float Pressure) override;

public:
	AHydraulicPiston();
};
