// HydroSchemes Simulator. All rights reserved.


#include "Tests/HSInputRecordingComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Tests/Utils/JsonUtils.h"
#include "GameFramework/PlayerInput.h"
#include "Player/HUBaseCharacter.h"
#include "InputMappingContext.h"

using namespace Test;

UHSInputRecordingComponent::UHSInputRecordingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
} 


void UHSInputRecordingComponent::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner());
	check(GetWorld());
	check(GetOwner()->InputComponent);

	const APawn* Pawn = Cast<APawn>(GetOwner());
	check(Pawn);

	const APlayerController* PlayerController = Pawn->GetController<APlayerController>();
	check(PlayerController);

	PlayerInput = Cast<UEnhancedPlayerInput>(PlayerController->PlayerInput);
	check(PlayerInput);

	InputData.InitialTransform = GetOwner()->GetActorTransform();
	InputData.Bindings.Add(MakeBindingsData(1.f / 60.f));
}

void UHSInputRecordingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	JsonUtils::WriteInputData(GenerateFileName(), InputData);
}


void UHSInputRecordingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	InputData.Bindings.Add(MakeBindingsData_Implementation(DeltaTime));
}

FBindingsData UHSInputRecordingComponent::MakeBindingsData_Implementation(float DeltaTime) const
{
	FBindingsData BindingsData;
	BindingsData.WorldTime = GetWorld()->TimeSeconds;
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(GetOwner()->InputComponent);
	AHUBaseCharacter* Character = GetOwner<AHUBaseCharacter>();
	APlayerController* PlayerController = Character->GetController<APlayerController>();
	UPlayerInput* ControllerPlayerInput = PlayerController->PlayerInput;
	float DeltaX = 0.f;
	float DeltaY = 0.f;
	PlayerController->GetInputMouseDelta(DeltaX, DeltaY);
	for (auto Mapping : Character->PlayerInputs.Get()->GetMappings()) {
		UE_LOG(LogTemp, Warning, TEXT("x = %f"), DeltaX);
		UE_LOG(LogTemp, Warning, TEXT("y = %f"), DeltaY);
		if (PlayerController->IsInputKeyDown(Mapping.Key)) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Mapping.Key.ToString());
			BindingsData.ActionValues.Add(
				FActionData{
					Mapping.Key,
					DeltaTime,
					PlayerController->GetInputVectorKeyState(Mapping.Key)
				});
		}
	}
	if (DeltaX || DeltaY) {
		bool bHasMouse2D = false;
		for (auto ActionValue : BindingsData.ActionValues) {
			if (ActionValue.Key == EKeys::Mouse2D) bHasMouse2D = true;
		}
		if (!bHasMouse2D) {
			BindingsData.ActionValues.Add(
				FActionData{
					EKeys::Mouse2D,
					DeltaTime,
					FVector(DeltaX / ControllerPlayerInput->GetMouseSensitivityX(), DeltaY / ControllerPlayerInput->GetMouseSensitivityY(), 0.f)
				});
		}
	}
	float showUELOG = true;
	/*for (const TUniquePtr<FEnhancedInputActionEventBinding>& Binding : EnhancedInputComponent->GetActionEventBindings()) {
		TObjectPtr<const UInputAction> DelegateAction = Binding->GetAction();
		if (const FInputActionInstance* ActionData = PlayerInput->FindActionInstanceData(DelegateAction)) {
			if (ActionData->GetValue().IsNonZero()) {
				//Binding->Execute(*ActionData);
				if (showUELOG) {
					UE_LOG(LogTemp, Warning, TEXT("START"));
					showUELOG = false;
				}
				UE_LOG(LogTemp, Warning, TEXT("YES %s - %s"), *DelegateAction.GetName(), *ActionData->GetValue().ToString());
				BindingsData.ActionValues.Add(
					FActionData{ 
						DelegateAction.GetName(), 
						(int32)Binding->GetTriggerEvent(), 
						ActionData->GetElapsedTime(), 
						ActionData->GetTriggeredTime(), 
						FVector(ActionData->GetValue()[0], ActionData->GetValue()[1], ActionData->GetValue()[2]), 
						(int32)ActionData->GetValue().GetValueType()});
			}
		}
	}*/
	return BindingsData;
}

FString UHSInputRecordingComponent::GenerateFileName() const
{
	return FPaths::GameSourceDir().Append("HydroSchemes/Private/Tests/Data/CharacterTestInput1.json");
}

