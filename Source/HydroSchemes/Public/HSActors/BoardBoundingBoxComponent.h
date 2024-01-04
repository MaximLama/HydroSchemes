// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BoardBoundingBoxComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HYDROSCHEMES_API UBoardBoundingBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UBoardBoundingBoxComponent();

	UPROPERTY(EditAnywhere)
	float BoxSizeLength = 200.0f;

	void IncreaseBoundingBox(const AActor* ChildActor);
	void UpdateBoxExtent();
	void ShiftBoundingBox(const FVector& Offset);

protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FBox BoundingBox;
};
