// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMainMenu.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API UMiniMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;
	
	UFUNCTION(BlueprintCallable)	
	void CloseWidget();

	UFUNCTION(BlueprintCallable)
	void OpenSettingsMenu();

	UFUNCTION(BlueprintCallable)
	void ExitGame();
};
