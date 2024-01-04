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
	UPROPERTY(EditDefaultsOnly)
	FVector InitialLocation;
	UPROPERTY(EditDefaultsOnly)
	FVector EndLocation;
	UPROPERTY(EditAnywhere)
	float TranslationDuration = 0.5f;
	UPROPERTY(EditAnywhere)
	float InterpSpeed = 1.0f;
	bool bIsTranslationActive = false;
	float TimeElapsed = 0.f;
	FVector FromLocation;
	FVector ToLocation;
	bool bInStart = true;
	

	void Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme);
	void CreateSchemes();
	bool FindTank(FString SocketName);
	void UpdateState();
	virtual void OnSetInputPressureAfter(FString SocketName, float Pressure) override;
	void MovePistonIfPossible();

public:
	AHydraulicPiston();
	virtual void Tick(float DeltaTime) override;
};
