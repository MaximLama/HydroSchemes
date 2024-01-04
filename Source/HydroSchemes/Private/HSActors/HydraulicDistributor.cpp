// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicDistributor.h"
#include "FunctionalComponents/LeverBoxComponent.h"

#define INTERACT ECC_GameTraceChannel1

AHydraulicDistributor::AHydraulicDistributor() 
{
	Lever = CreateDefaultSubobject<UStaticMeshComponent>("Lever");
	Lever->SetupAttachment(RootComponent);

	LeverBox = CreateDefaultSubobject<ULeverBoxComponent>("LeverBox");
	LeverBox->SetupAttachment(Lever);
	LeverBox->SetCollisionResponseToChannel(INTERACT, ECR_Block);
}
