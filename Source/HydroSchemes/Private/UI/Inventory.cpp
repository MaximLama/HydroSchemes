// HydroSchemes Simulator. All rights reserved.


#include "UI/Inventory.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HUPlayerController.h"
#include "UI/InventorySlot.h"
#include "Engine/Engine.h"

void UInventory::AddItemToList(const FInventoryElement& InventoryElement)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, InventoryElement.ElementName.ToString());
	}
	if (!GetWorld()) return;
	AHUPlayerController* PlayerController = Cast<AHUPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController && InventorySlotClass) {
		auto InventorySlot = CreateWidget<UInventorySlot>(PlayerController, InventorySlotClass);
		if (ItemsList) {
			ItemsList->AddChild(InventorySlot);
			InventorySlot->SetInventoryItem(InventoryElement);
		}
	}
}

void UInventory::NativeConstruct()
{
	Super::NativeConstruct();
}
