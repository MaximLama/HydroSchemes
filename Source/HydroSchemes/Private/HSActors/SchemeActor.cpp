// HydroSchemes Simulator. All rights reserved.


#include "HSActors/SchemeActor.h"
#include "Inventory/InventoryElement.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/HUBaseCharacter.h"

#define ECC_Interact ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogSchemeActor, All, All)

// Sets default values
ASchemeActor::ASchemeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;
}

void ASchemeActor::PickUpItem(FInventoryElement& InventoryElement)
{
	UE_LOG(LogSchemeActor, Warning, TEXT("ClassName - %s"), *ActorName.ToString());
	InventoryElement.ElementClass = GetClass();
	InventoryElement.ElementName = ActorName;
	Destroy();
}

// Called when the game starts or when spawned
void ASchemeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASchemeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASchemeActor::SetHoloMaterialInstance(TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstance)
{
	HoloMaterialInstance = MaterialInstance;
	
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* SMC : StaticMeshComponents) {
		if (SMC) {
			for (int32 i = 0; i < SMC->GetNumMaterials(); i++) {
				SMC->SetMaterial(i, HoloMaterialInstance.Get());
			}
		}
	}

}

void ASchemeActor::SetHoloMaterialColor(FColor Color)
{
	HoloMaterialInstance->SetVectorParameterValue(FName("Color"), Color);
}

void ASchemeActor::SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse){}

void ASchemeActor::SetRCT(){}

void ASchemeActor::AttachToCharacter(AHUBaseCharacter* Character, const FAttachmentTransformRules& AttachmentRules, FName SocketName)
{
	AttachToComponent(Character->GetMesh(), AttachmentRules, SocketName);
}

void ASchemeActor::ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult)
{
}

ASchemeActor* ASchemeActor::CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial)
{
	return nullptr;
}

void ASchemeActor::PlaceElement(AHUBaseCharacter* Character){}

void ASchemeActor::DisableCollision()
{
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* SMC : StaticMeshComponents) {
		if (SMC) {
			SMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
