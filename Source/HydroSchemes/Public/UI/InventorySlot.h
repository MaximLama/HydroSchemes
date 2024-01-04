// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryElement.h"
#include "InventorySlot.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
private:
	inline void SetInventoryItemTitle();

	FInventoryElement InventoryItem;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> InventoryItemTitle;

public:
	UFUNCTION(BlueprintCallable)
	void SpawnDetail();

	void SetInventoryItem(const FInventoryElement& InventoryElement);

};
