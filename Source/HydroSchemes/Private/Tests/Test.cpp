// HydroSchemes Simulator. All rights reserved.

#if (WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)

#include "Tests/Test.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "HSActors/Board.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "HSActors/Board.h"
#include "Tests/Utils/JsonUtils.h"
#include "Tests/Utils/InputRecordingUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Player/HUBaseCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

using namespace Test;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FWorldCanBeCreated,
	"Hydroschemes.WorldCanBeCreated",
	EAutomationTestFlags::ApplicationContextMask |
	EAutomationTestFlags::ProductFilter |
	EAutomationTestFlags::HighPriority
)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBoardSchemeActorsCanBeCreated, 
	"Hydroschemes.BoardSchemeActorsCanBeCreated", 
	EAutomationTestFlags::ApplicationContextMask | 
	EAutomationTestFlags::ProductFilter | 
	EAutomationTestFlags::HighPriority
)

class LevelScope {
	FString LevelName = "/Game/Maps/SchemeMap";

public:
	LevelScope(const FString& MapName) {
		AutomationOpenMap(MapName);
	}
	~LevelScope() {
		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);
	}
};

bool FWorldCanBeCreated::RunTest(const FString& Parameters) {
	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestTrueExpr((bool)World);
	return true;
}

class FSimulatedMovementLatentCommand : public IAutomationLatentCommand
{
public:
	FSimulatedMovementLatentCommand(UWorld* InWorld, UEnhancedInputComponent* InInputComponent, UInputMappingContext* InPlayerInput, const TArray<FBindingsData>& InBindingsData, APlayerController* InPlayerController)
		:World(InWorld), InputComponent(InInputComponent), PlayerInput(InPlayerInput), BindingsData(InBindingsData), PlayerController(InPlayerController)
	{

	}

	virtual bool Update() {
		if (!World || !InputComponent) return true;
		if (WorldStartTime == 0.0f) {
			WorldStartTime = World->TimeSeconds;
		}

		while (World->TimeSeconds - WorldStartTime >= BindingsData[Index].WorldTime) {
			if (BindingsData[Index].ActionValues.IsEmpty()) {
				++Index;
				if (++Index >= BindingsData.Num()) return true;
				ActionDataArray = BindingsData[Index].ActionValues;
				continue;
			}

			for (const auto ActionDataTemp1 : ActionDataArray) {
				bool bIsInNow = false;
				for (const auto ActionDataTemp2 : BindingsData[Index].ActionValues) {
					if (ActionDataTemp1.Key == ActionDataTemp2.Key) {
						bIsInNow = true;
					}
				}
				if (!bIsInNow) {
					FInputKeyParams Params;
					Params.Delta = ActionDataTemp1.InputVector;
					Params.DeltaTime = ActionDataTemp1.DeltaTime;
					Params.Key = ActionDataTemp1.Key;
					Params.Event = EInputEvent::IE_Released;
					PlayerController->InputKey(Params);
				}
			}
			
			for (const auto ActionData : BindingsData[Index].ActionValues) {
				FInputKeyParams Params;
				Params.Delta = ActionData.InputVector;
				Params.DeltaTime = ActionData.DeltaTime;
				Params.Key = ActionData.Key;
				bool bHasPrevInput = false;
				for (auto ad : ActionDataArray) {
					if (ad.Key == ActionData.Key) {
						bHasPrevInput = true;
						LastActionData = ad;
					}
				}
				if (bHasPrevInput) {
					Params.Event = ActionData.Key == EKeys::Mouse2D ? EInputEvent::IE_Axis : EInputEvent::IE_Repeat;
				}
				else {
					Params.Event = ActionData.Key == EKeys::Mouse2D ? EInputEvent::IE_Axis : EInputEvent::IE_Pressed;
				}
				UE_LOG(LogTemp, Warning, TEXT("%s"), *ActionData.Key.ToString());
				PlayerController->InputKey(Params);
				
				/*for (auto Mapping : PlayerInput->GetMappings()) {
					if (Mapping.Action.GetName() == ActionData.IA_Name) {
						FEnhancedInputActionEventDelegateBinding Binding = FEnhancedInputActionEventDelegateBinding<FEnhancedInputActionHandlerValueSignature>(Mapping.Action, ETriggerEvent(ActionData.TriggerEvent));
						Binding.Execute(FInputActionDataInstanceChild(
							Mapping.Action,
							ActionData.ElapsedProcessedTime,
							ActionData.ElapsedTriggeredTime,
							FInputActionValue(ActionData.Value),
							ETriggerEvent(ActionData.TriggerEvent)));
					}
				}*/
				
			}
			ActionDataArray = BindingsData[Index].ActionValues;
			if (++Index >= BindingsData.Num()) return true;
		}
		return false;
	}
private:
	const UWorld* World;
	const UEnhancedInputComponent* InputComponent;
	const UInputMappingContext* PlayerInput;
	const TArray<FBindingsData> BindingsData;
	int32 Index{ 0 };
	float WorldStartTime{ 0.0f };
	APlayerController* PlayerController;
	TArray<FActionData> ActionDataArray = TArray<FActionData>();
	FActionData LastActionData;
};

FString GetTestDataDir() {
	return FPaths::GameSourceDir().Append("HydroSchemes/Private/Tests/Data/");
}

bool FBoardSchemeActorsCanBeCreated::RunTest(const FString& Parameters)
{
	const auto Level = LevelScope("/Game/Maps/SchemeMap");
	UWorld* World = AutomationCommon::GetAnyGameWorld();
	if (!TestNotNull("World exists", World)) return false;
	AHUBaseCharacter* Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	if (!TestNotNull("Character exists", Character)) return false;
	
	const FString FileName = FPaths::GameSourceDir().Append("HydroSchemes/Private/Tests/Data/CharacterTestInput1.json");
	FInputData InputData;
	if(!JsonUtils::ReadInputData(FileName, InputData)) return false;
	if (!TestTrue("Input data is not empty", InputData.Bindings.Num() > 0)) return false;

	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (!TestNotNull("Player controller exists", PlayerController)) return false;

	Character->SetActorTransform(InputData.InitialTransform);
	PlayerController->SetControlRotation(InputData.InitialTransform.Rotator());
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerController->InputComponent);
	if (!TestNotNull("Player Input Mapping Exists", Character->PlayerInputs.Get())) return false;
	ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
	ADD_LATENT_AUTOMATION_COMMAND(FSimulatedMovementLatentCommand(World, EnhancedInputComponent, Character->PlayerInputs.Get(), InputData.Bindings, PlayerController));

	return true;
}

#endif