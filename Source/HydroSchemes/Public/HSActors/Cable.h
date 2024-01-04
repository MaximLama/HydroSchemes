// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/SchemeActor.h"
#include "Cable.generated.h"

#define BOARD_ACTOR_CHANNEL ECC_GameTraceChannel3

class UCableComponent;
class UStaticMeshComponent;
class ABoardSchemeActor;
struct FBoardActorOutput;

/**
 * 
 */
UCLASS()
class HYDROSCHEMES_API ACable : public ASchemeActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> CableInputLeft;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> CableInputRight;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCableComponent> Cable;
	
	UPROPERTY(EditAnywhere)
	FTransform CableLeftInitialTransform;

	UPROPERTY(EditAnywhere)
	FTransform CableRightInitialTransform;

public:
	bool bIsCableLeftFixed = false;

	ABoardSchemeActor* CableRightAttachActor;
	FName CableRightAttachSocketName;

	ACable();

	virtual void Tick(float DeltaTime) override;
	virtual void AttachToCharacter(AHUBaseCharacter* Character, const FAttachmentTransformRules& AttachmentRules, FName SocketName) override;
	virtual void ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult) override;
	virtual ASchemeActor* CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial) override;
	void InitializeForm();
	void AttachToBoardActor(ABoardSchemeActor* SchemeActor, const FAttachmentTransformRules& AttachmentRules, const FName SocketName);
	virtual void SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse) override;
	virtual void SetRCT() override;
	virtual void SetHoloMaterialInstance(TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstance) override;
	FName GetNearestSocketName(ABoardSchemeActor* SchemeActor, FVector ImpactPoint);
	virtual void PlaceElement(AHUBaseCharacter* Character) override;
};
