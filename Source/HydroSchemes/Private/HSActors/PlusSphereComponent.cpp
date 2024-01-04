// HydroSchemes Simulator. All rights reserved.

#include "HSActors/PlusSphereComponent.h"
#include "HSActors/Plus.h"

void UPlusSphereComponent::Interact()
{
	if (GetOwner()) {
		Cast<APlus>(GetOwner())->Interact();
	}
}
