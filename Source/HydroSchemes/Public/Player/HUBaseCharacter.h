// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Inventory/InventoryElement.h"
#include "InputActionValue.h"
#include "HUBaseCharacter.generated.h"

struct FInventoryElement;
struct FBoardActorOutput;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UInventoryComponent;
class ABoardSchemeActor;

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	Game UMETA(DisplayName = "Режим игры"),
	UI UMETA(DisplayName = "Режим пользовательского интерфейса")
};

enum class EHoloState : uint8
{
	Deleted UMETA(DisplayName = "Удален"),
	Correct UMETA(DisplayName = "Некорректное расположение"),
	Incorrect UMETA(DisplayName = "Корректное расположение")
};


UCLASS()
class HYDROSCHEMES_API AHUBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHUBaseCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UInventory> InventoryWidgetClass;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventory> InventoryWidget;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FName PlacingElementAttachSocketName = "HSItemSocket";

	UPROPERTY()
	TObjectPtr<ASchemeActor> PlacingElement = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> HoloMaterial;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerInputs;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DeleteHoloAction;

	UPROPERTY(EditAnywhere, Category = "Holo")
	FColor CorrectHoloColor;

	UPROPERTY(EditAnywhere, Category = "Holo")
	FColor IncorrectHoloColor;

	TObjectPtr<ASchemeActor> Holo;

	UFUNCTION(BlueprintCallable)
	void SpawnActorInHand(const FInventoryElement& InventoryElement);

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void ShowHolo();

	void SetHoloState(const EHoloState State);
	void CheckHoloState(const ABoardPart* AttachBoardPart);
	void CheckHoloState(ABoardSchemeActor* AttachBoardSchemeActor, const FName SocketOutputName);
	void DestroyHolo();

private:
	EInputMode InputMode = EInputMode::Game;
	EHoloState HoloState = EHoloState::Deleted;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void ToggleInventory();
	UFUNCTION()
	void DeleteHolo();
	void PlaceElement();
};
