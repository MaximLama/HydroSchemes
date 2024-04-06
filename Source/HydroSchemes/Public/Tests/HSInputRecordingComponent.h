// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tests/Utils/InputRecordingUtils.h"
#include "HSInputRecordingComponent.generated.h"

class UEnhancedPlayerInput;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYDROSCHEMES_API UHSInputRecordingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHSInputRecordingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	FInputData InputData;

	UFUNCTION(BlueprintNativeEvent)
	FBindingsData MakeBindingsData(float DeltaTime) const;

	virtual FBindingsData MakeBindingsData_Implementation(float DeltaTime) const;

	FString GenerateFileName() const;

	UPROPERTY()
	UEnhancedPlayerInput* PlayerInput;
};
