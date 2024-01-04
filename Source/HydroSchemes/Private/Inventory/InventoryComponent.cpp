// HydroSchemes Simulator. All rights reserved.


#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryElement.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UInventoryComponent::AddItemToInventory(FInventoryElement& SchemeActor)
{
	Inventory.Add(SchemeActor);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
