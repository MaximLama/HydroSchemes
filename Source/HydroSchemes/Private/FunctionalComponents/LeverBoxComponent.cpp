// HydroSchemes Simulator. All rights reserved.


#include "FunctionalComponents/LeverBoxComponent.h"
#include "HSActors/BoardSchemeActor.h"

ULeverBoxComponent::ULeverBoxComponent()
{
	SetIsInteractable(false);
	PrimaryComponentTick.bCanEverTick = true;
}

void ULeverBoxComponent::Interact()
{
	auto LeverComponent = GetAttachParent();
	if (LeverComponent) {
		UStaticMeshComponent* Lever = Cast<UStaticMeshComponent>(LeverComponent);
		StartTransform = LeverStates[StateIndex].LeverState;
		EndTransform = LeverStates[(StateIndex + 1) % LeverStates.Num()].LeverState;
		bIsRotationActive = true;
	}
}

int32 ULeverBoxComponent::GetStateIndex()
{
	return StateIndex;
}

void ULeverBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsRotationActive) {
		TimeElapsed += DeltaTime;

		float Alpha = FMath::Clamp(TimeElapsed * InterpSpeed / RotationDuration, 0.f, 1.f);
		FVector NewLocation = FMath::Lerp(StartTransform.GetLocation(), EndTransform.GetLocation(), Alpha);
		FRotator NewRotation = FMath::Lerp(StartTransform.GetRotation().Rotator(), EndTransform.GetRotation().Rotator(), Alpha);
		FTransform NewTransform(NewRotation, NewLocation, EndTransform.GetScale3D());
		auto LeverComponent = GetAttachParent();
		if (LeverComponent) {
			UStaticMeshComponent* Lever = Cast<UStaticMeshComponent>(LeverComponent);
			Lever->SetRelativeTransform(NewTransform);
		}
		if (TimeElapsed >= (RotationDuration / InterpSpeed)) {
			bIsRotationActive = false;
			TimeElapsed = 0.0f;
			StateIndex = (StateIndex + 1) % LeverStates.Num();
			ABoardSchemeActor* OwnerBoardActor = Cast<ABoardSchemeActor>(GetOwner());
			if (OwnerBoardActor) {
				OwnerBoardActor->ChangeState();
			}
		}
	}
}
