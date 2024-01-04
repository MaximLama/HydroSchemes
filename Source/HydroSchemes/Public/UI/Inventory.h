// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryElement.h"
#include "Inventory.generated.h"

class UVerticalBox;
class UInventorySlot;

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UInventory : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> ItemsList;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlot> InventorySlotClass;

	void AddItemToList(const FInventoryElement& InventoryElement);

	virtual void NativeConstruct() override;
};
