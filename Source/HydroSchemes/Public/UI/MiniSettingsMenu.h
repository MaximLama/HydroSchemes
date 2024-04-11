// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniSettingsMenu.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UMiniSettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION(BlueprintCallable)
	void CloseWidget();
};
