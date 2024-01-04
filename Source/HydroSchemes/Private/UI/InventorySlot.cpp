// HydroSchemes Simulator. All rights reserved.


#include "UI/InventorySlot.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HUBaseCharacter.h"
#include "Inventory/InventoryComponent.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Visible);
	if (InventoryItemTitle && !InventoryItemTitle->GetText().IsEmpty()) {
		InventoryItemTitle->SetText(FText::FromString("Inventory Item Name"));
	}
}

void UInventorySlot::SpawnDetail()
{
	AHUBaseCharacter* Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Character) {
		Character->SpawnActorInHand(InventoryItem);
	}
}

void UInventorySlot::SetInventoryItemTitle()
{
	if (InventoryItem.ElementClass && InventoryItem.ElementName != FName("") || InventoryItem.ElementName != NAME_None) {
		InventoryItemTitle->SetText(FText::FromString(InventoryItem.ElementName.ToString()));
	}
}

void UInventorySlot::SetInventoryItem(const FInventoryElement& InventoryElement)
{
	InventoryItem = InventoryElement;
	SetInventoryItemTitle();
}
