// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputRecordingUtils.generated.h"

USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_BODY()

	/*UPROPERTY()
	FString IA_Name;

	UPROPERTY()
	int32 TriggerEvent;

	UPROPERTY()
	float ElapsedProcessedTime;

	UPROPERTY()
	float ElapsedTriggeredTime;

	UPROPERTY()
	FVector Value;

	UPROPERTY()
	int32 ValueType;*/

	UPROPERTY(BlueprintReadWrite)
	FKey Key;

	UPROPERTY(BlueprintReadWrite)
	float DeltaTime;

	UPROPERTY(BlueprintReadWrite)
	FVector InputVector;

	UPROPERTY(BlueprintReadWrite)
	int32 InputDevice;
};

USTRUCT(BlueprintType)
struct FBindingsData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FActionData> ActionValues;

	UPROPERTY(BlueprintReadWrite)
	float WorldTime{ 0.0f };
};

USTRUCT()
struct FInputActionDataInstanceChild : public FInputActionInstance{
	GENERATED_BODY()
public:
	FInputActionDataInstanceChild(
		const UInputAction* InSourceAction, 
		float ElapsedProcessedTime = 0, 
		float ElapsedTriggeredTime = 0, 
		FInputActionValue Value = 0, 
		ETriggerEvent TriggerEvent = ETriggerEvent::None): FInputActionInstance(InSourceAction){
		this->ElapsedProcessedTime = ElapsedProcessedTime;
		this->ElapsedTriggeredTime = ElapsedTriggeredTime;
		this->Value = Value;
		this->TriggerEvent = TriggerEvent;
	}
	FInputActionDataInstanceChild() = default;
};

USTRUCT()
struct FInputData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBindingsData> Bindings;

	UPROPERTY()
	FTransform InitialTransform;
};