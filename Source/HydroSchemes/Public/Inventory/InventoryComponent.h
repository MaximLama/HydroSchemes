// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

struct FInventoryElement;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYDROSCHEMES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<FInventoryElement> Inventory;

	void AddItemToInventory(FInventoryElement& SchemeActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
