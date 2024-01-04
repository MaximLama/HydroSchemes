// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "LeverBoxComponent.generated.h"

USTRUCT()
struct FLeverState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FTransform LeverState;
};

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API ULeverBoxComponent : public UBoxComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FLeverState> LeverStates;

	UPROPERTY(EditAnywhere)
	float RotationDuration = 0.5f;
	UPROPERTY(EditAnywhere)
	float InterpSpeed = 1.0f;

	ULeverBoxComponent();
	virtual void Interact() override;
	int32 GetStateIndex();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsRotationActive = false;
	float TimeElapsed = 0.f;
	int32 StateIndex = 0;
	FTransform StartTransform;
	FTransform EndTransform;
};
