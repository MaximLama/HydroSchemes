// HydroSchemes Simulator. All rights reserved.


#include "HSActors/BoardBoundingBoxComponent.h"
#include "HSActors/BoardPart.h"
#include "Components/StaticMeshComponent.h"

UBoardBoundingBoxComponent::UBoardBoundingBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBoardBoundingBoxComponent::BeginPlay() 
{
	Super::BeginPlay();
}

void UBoardBoundingBoxComponent::IncreaseBoundingBox(const AActor* ChildActor)
{
	FTransform ActorTransform = GetOwner()->GetActorTransform();
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	ChildActor->GetComponents<UStaticMeshComponent*>(StaticMeshComponents);
	if (StaticMeshComponents.Num() > 0) {
		for (auto MeshComponent : StaticMeshComponents) {
			FBox ComponentBoundingBox = MeshComponent->Bounds.GetBox();
			ComponentBoundingBox = ComponentBoundingBox.TransformBy(ActorTransform);
			BoundingBox += ComponentBoundingBox;

		}
	}
}

void UBoardBoundingBoxComponent::UpdateBoxExtent()
{
	FVector Extent = BoundingBox.GetExtent();
	Extent.Z = (Extent.Z > BoxSizeLength / 2) ? Extent.Z : BoxSizeLength / 2;
	SetBoxExtent(Extent);
}

void UBoardBoundingBoxComponent::ShiftBoundingBox(const FVector& Offset)
{
	if (GetOwner())	BoundingBox = BoundingBox.ShiftBy(Offset);
}
