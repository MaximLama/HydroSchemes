// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/BoardSchemeActor.h"
#include "HydraulicPump.generated.h"

class ULeverBoxComponent;

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API AHydraulicPump : public ABoardSchemeActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Lever;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ULeverBoxComponent> LeverBox;

public:
	UPROPERTY(EditAnywhere)
	float DefaultPressure = 200.f;

	AHydraulicPump();
	virtual void ChangeState() override;
};
