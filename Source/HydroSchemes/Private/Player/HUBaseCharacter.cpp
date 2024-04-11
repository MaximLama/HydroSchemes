// HydroSchemes Simulator. All rights reserved.


#include "Player/HUBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "HSActors/Board.h"
#include "HSActors/BoardPart.h"
#include "HSActors/Plus.h"
#include "HSActors/SchemeActor.h"
#include "Interfaces/InteractableInterface.h"
#include "Interfaces/PickUpInterface.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HUPlayerController.h"
#include "UI/Inventory.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HSActors/BoardSchemeActor.h"
#include "HSActors/Cable.h"
#include "UI/MiniMainMenu.h"
#include "UI/MiniSettingsMenu.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All)

#define INTERACT ECC_GameTraceChannel1

// Sets default values
AHUBaseCharacter::AHUBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetMesh(), "head");
	CameraComponent->bUsePawnControlRotation = true;
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");	
}

// Called when the game starts or when spawned
void AHUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Добавить контекст пользовательских действий
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(PlayerInputs, 0);
		}
	}

	//Добавить виджет инвентаря
	if (!GetWorld()) return;
	
	if (IsPlayerControlled() && InventoryWidgetClass) {
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController) {
			InventoryWidget = CreateWidget<UInventory>(PlayerController, InventoryWidgetClass);
			InventoryWidget->AddToViewport(1);
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

// Called every frame
void AHUBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShowHolo();
}



// Called to bind functionality to input
void AHUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::Interact);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(DeleteHoloAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::DeleteHolo);
		EnhancedInputComponent->BindAction(ToggleMiniMainMenuAction, ETriggerEvent::Triggered, this, &AHUBaseCharacter::ToggleMiniMainMenu);
	}
}

void AHUBaseCharacter::Interact()
{
	if (InputMode == EInputMode::UI) return;
	if (HoloState == EHoloState::Deleted) {
		FVector ViewLocation;
		FRotator ViewRotation;

		Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FVector TraceStart = ViewLocation;
		FVector TraceEnd = TraceStart + ViewRotation.Vector() * 1500.0f;

		FHitResult HitResult;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, INTERACT)) {
			if (!HitResult.bBlockingHit) return;
			auto HitActor = HitResult.GetActor();
			auto HitComponent = HitResult.GetComponent();
			if (HitActor->Implements<UInteractableInterface>()) {
				IInteractableInterface* InteractActor = Cast<IInteractableInterface>(HitActor);
				if(InteractActor->bIsInteractable) InteractActor->Interact();
			}
			if (HitComponent->Implements<UInteractableInterface>()) {
				IInteractableInterface* InteractActor = Cast<IInteractableInterface>(HitComponent);
				if (InteractActor->bIsInteractable) InteractActor->Interact();
			}
			if (HitActor->Implements<UPickUpInterface>()) {
				IPickUpInterface* PickUpActor = Cast<IPickUpInterface>(HitActor);
				if (PickUpActor->bIsPickUpAble) {
					auto InventoryElement = FInventoryElement();
					PickUpActor->PickUpItem(&InventoryElement);
					InventoryComponent->AddItemToInventory(InventoryElement);
					if (InventoryWidget && InventoryWidget->IsValidLowLevel())
						InventoryWidget->AddItemToList(InventoryElement);
				}
			}
		}
	}
	else {
		PlaceElement();
	}
}

void AHUBaseCharacter::SpawnActorInHand(const FInventoryElement& InventoryElement)
{
	if (!InventoryElement.ElementClass || InventoryElement.ElementName == "") return;
	if (!GetWorld()) return;
	if (!PlacingElement && PlacingElement->StaticClass() == InventoryElement.ElementClass) return;
	if (PlacingElement) {
		PlacingElement->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PlacingElement->Destroy();
	}
	if (!PlacingElement || PlacingElement->IsActorBeingDestroyed()) {
		PlacingElement = GetWorld()->SpawnActor<ASchemeActor>(InventoryElement.ElementClass);
		PlacingElement->SetResponseToChannel(ECC_Pawn, ECR_Ignore);
		PlacingElement->DisableCollision();
		PlacingElement->AttachToCharacter(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PlacingElementAttachSocketName);
	}
	if(Holo && !Holo->IsActorBeingDestroyed()) {
		DestroyHolo();
	}
}

void AHUBaseCharacter::ShowHolo()
{
	if (PlacingElement && !PlacingElement->IsActorBeingDestroyed() && InputMode == EInputMode::Game) {
		FVector ViewLocation;
		FRotator ViewRotation;

		Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FVector TraceStart = ViewLocation;
		FVector TraceEnd = TraceStart + ViewRotation.Vector() * 1500.0f;

		FHitResult HitResult;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, PlacingElement->CollisionChannelToPlace)) {
			if (!HitResult.bBlockingHit) return;
			auto HitActor = HitResult.GetActor();
			if (HitActor->IsA(PlacingElement->ActorsToPlace)) {
				PlacingElement->ShowHolo(this, HitResult);
			}
		}
	}
}

void AHUBaseCharacter::DestroyHolo()
{
	if (Holo && !Holo->IsActorBeingDestroyed()) {
		Holo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Holo->Destroy();
	}
}

void AHUBaseCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AHUBaseCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AHUBaseCharacter::ToggleInventory()
{
	if (InputMode == EInputMode::Game) {
		if (InventoryWidget && InventoryWidget->IsValidLowLevel()) {
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				PlayerController->SetIgnoreLookInput(true);
				PlayerController->SetIgnoreMoveInput(true);
				FInputModeGameAndUI InputGameUIMode;
				InputGameUIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputGameUIMode.SetHideCursorDuringCapture(false);
				PlayerController->SetInputMode(InputGameUIMode);
				PlayerController->SetShowMouseCursor(true);
				InputMode = EInputMode::UI;
			}
		}
	}
	else {
		if (InventoryWidget && InventoryWidget->IsValidLowLevel()) {
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				FInputModeGameOnly InputGameMode;
				PlayerController->SetInputMode(InputGameMode);
				PlayerController->SetShowMouseCursor(false);
				PlayerController->SetIgnoreLookInput(false);
				PlayerController->SetIgnoreMoveInput(false);
				InputMode = EInputMode::Game;
			}
		}
	}
}

void AHUBaseCharacter::ToggleMiniMainMenu()
{
	if (InputMode == EInputMode::Game) {

		if (!MiniMainMenuWidget){
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				MiniMainMenuWidget = CreateWidget<UMiniMainMenu>(PlayerController, MiniMainMenuWidgetClass);
				MiniMainMenuWidget->AddToViewport(1);
				PlayerController->SetIgnoreLookInput(true);
				PlayerController->SetIgnoreMoveInput(true);
				FInputModeGameAndUI InputGameUIMode;
				InputGameUIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputGameUIMode.SetHideCursorDuringCapture(false);
				PlayerController->SetInputMode(InputGameUIMode);
				PlayerController->SetShowMouseCursor(true);
				InputMode = EInputMode::UI;
			}
		}
	}
	else {
		if (MiniMainMenuWidget) {
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				FInputModeGameOnly InputGameMode;
				PlayerController->SetInputMode(InputGameMode);
				PlayerController->SetShowMouseCursor(false);
				PlayerController->SetIgnoreLookInput(false);
				PlayerController->SetIgnoreMoveInput(false);
				InputMode = EInputMode::Game;
				MiniMainMenuWidget->RemoveFromViewport();
				MiniMainMenuWidget = nullptr;
			}
		}
	}
}

void AHUBaseCharacter::ToggleMiniSettingsMenu()
{
	if (InputMode == EInputMode::Game) {

		if (!MiniSettingsMenuWidget) {
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				MiniSettingsMenuWidget = CreateWidget<UMiniSettingsMenu>(PlayerController, MiniSettingsMenuWidgetClass);
				MiniSettingsMenuWidget->AddToViewport(1);
				PlayerController->SetIgnoreLookInput(true);
				PlayerController->SetIgnoreMoveInput(true);
				FInputModeGameAndUI InputGameUIMode;
				InputGameUIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputGameUIMode.SetHideCursorDuringCapture(false);
				PlayerController->SetInputMode(InputGameUIMode);
				PlayerController->SetShowMouseCursor(true);
				InputMode = EInputMode::UI;
			}
		}
	}
	else {
		if (MiniSettingsMenuWidget) {
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
				FInputModeGameOnly InputGameMode;
				PlayerController->SetInputMode(InputGameMode);
				PlayerController->SetShowMouseCursor(false);
				PlayerController->SetIgnoreLookInput(false);
				PlayerController->SetIgnoreMoveInput(false);
				InputMode = EInputMode::Game;
				MiniSettingsMenuWidget->RemoveFromViewport();
				MiniSettingsMenuWidget = nullptr;
			}
		}
	}
}

void AHUBaseCharacter::DeleteHolo()
{
	if (InputMode == EInputMode::Game && HoloState > EHoloState::Deleted) {
		DestroyHolo();
		if (PlacingElement && !PlacingElement->IsActorBeingDestroyed()) {
			PlacingElement->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			PlacingElement->Destroy();
		}
		HoloState = EHoloState::Deleted;
	}
}

void AHUBaseCharacter::PlaceElement()
{
	if (!GetWorld()) return;
	if (PlacingElement && !PlacingElement->IsActorBeingDestroyed() && HoloState == EHoloState::Correct) {
		PlacingElement->PlaceElement(this);
	}
}

void AHUBaseCharacter::CheckHoloState(const ABoardPart* AttachBoardPart)
{
	if (auto BoardHoloSchemeActor = Cast<ABoardSchemeActor>(Holo)) {
		auto Board = Cast<ABoard>(AttachBoardPart->GetAttachParentActor());
		if (Board) {
			BoardHoloSchemeActor->InitSize(Board->GetBoardPartSize().X);
			if (Board->CanPlaceHolo(BoardHoloSchemeActor)) {
				BoardHoloSchemeActor->SetHoloMaterialColor(CorrectHoloColor);
				HoloState = EHoloState::Correct;
			}
			else {
				BoardHoloSchemeActor->SetHoloMaterialColor(IncorrectHoloColor);
				HoloState = EHoloState::Incorrect;
			}
		}
	}
}

void AHUBaseCharacter::CheckHoloState(ABoardSchemeActor* AttachBoardSchemeActor, const FName SocketOutputName)
{
	if (auto CableHolo = Cast<ACable>(Holo)) {
		if (AttachBoardSchemeActor->CanPlaceHolo(SocketOutputName)) {
			if (CableHolo->bIsCableLeftFixed) {
				auto ParentActor = Cast<ABoardSchemeActor>(CableHolo->GetAttachParentActor());
				if (ParentActor && (ParentActor == AttachBoardSchemeActor) && (CableHolo->GetAttachParentSocketName() == SocketOutputName)) {
					CableHolo->SetHoloMaterialColor(IncorrectHoloColor);
					HoloState = EHoloState::Incorrect;
					return;
				}
			}

			CableHolo->SetHoloMaterialColor(CorrectHoloColor);
			HoloState = EHoloState::Correct;
		}
		else {
			CableHolo->SetHoloMaterialColor(IncorrectHoloColor);
			HoloState = EHoloState::Incorrect;
		}
	}
}

void AHUBaseCharacter::SetHoloState(const EHoloState State)
{
	HoloState = State;
}
