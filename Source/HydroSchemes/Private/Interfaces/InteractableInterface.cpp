// HydroSchemes Simulator. All rights reserved.


#include "Interfaces/InteractableInterface.h"

// Add default functionality here for any IInteractableInterface functions that are not pure virtual.

void IInteractableInterface::SetIsInteractable(bool Value)
{
	bIsInteractable = Value;
}
