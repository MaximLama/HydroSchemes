// HydroSchemes Simulator. All rights reserved.


#include "UI/MiniSettingsMenu.h"
#include "Components/Button.h"
#include"Player/HUBaseCharacter.h"
#include"Player/HUPlayerController.h"
#include"Kismet/GameplayStatics.h"

void UMiniSettingsMenu::CloseWidget()
{
	AHUBaseCharacter* Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)return;
	Character->ToggleMiniSettingsMenu();
	Character->ToggleMiniMainMenu();
}
