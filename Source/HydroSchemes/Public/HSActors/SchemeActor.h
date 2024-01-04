// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PickUpInterface.h"
#include "SchemeActor.generated.h"

#define BOARD_CHANNEL ECC_GameTraceChannel2

class AHUBaseCharacter;
class UMaterialInstanceDynamic;

UCLASS(Blueprintable)
class HYDROSCHEMES_API ASchemeActor : public AActor, public IPickUpInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASchemeActor();

	UPROPERTY(EditDefaultsOnly, Category="InventoryData")
	FName ActorName;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	TWeakObjectPtr<UMaterialInstanceDynamic> HoloMaterialInstance;

	virtual void PickUpItem(FInventoryElement* InventoryElement) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannelToPlace;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorsToPlace;
	bool bIsOnBoard = false;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetHoloMaterialInstance(TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstance);
	void SetHoloMaterialColor(FColor Color);
	virtual void SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse);
	virtual void SetRCT();
	virtual void AttachToCharacter(AHUBaseCharacter* Character, const FAttachmentTransformRules& AttachmentRules, FName SocketName);
	virtual void ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult);
	virtual ASchemeActor* CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial);
	virtual void PlaceElement(AHUBaseCharacter* Character);
	void DisableCollision();
};
