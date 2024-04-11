// HydroSchemes Simulator. All rights reserved.


#include "UI/MiniMainMenu.h"
#include "Components/Button.h"
#include"Player/HUBaseCharacter.h"
#include"Player/HUPlayerController.h"
#include"Kismet/GameplayStatics.h"

void UMiniMainMenu::CloseWidget()
{
	AHUBaseCharacter* Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); 
	if (!Character)return;
	Character->ToggleMiniMainMenu();
}

void UMiniMainMenu::OpenSettingsMenu()
{

	AHUBaseCharacter *Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)return;
	Character->ToggleMiniMainMenu();
}

void UMiniMainMenu::ExitGame()
{
	AHUPlayerController* Controller = Cast<AHUPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!Controller)return;
	Controller->ConsoleCommand("quit");
}

//GetWorld(); - ��������� ����
 
//	AHUBaseCharacter *Character = Cast<AHUBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); //- ��������� ��������� (�������� �� ���������� ������� �������)
//if (!Character)return;

//AHUPlayerController* Controller = Cast<AHUPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); //- ��������� ����������� (�������� �� ���� � �������)
//if (!Controller)return;