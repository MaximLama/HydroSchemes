// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicPump.h"
#include "FunctionalComponents/LeverBoxComponent.h"

#define INTERACT ECC_GameTraceChannel1

AHydraulicPump::AHydraulicPump()
{
	Lever = CreateDefaultSubobject<UStaticMeshComponent>("Lever");
	Lever->SetupAttachment(RootComponent);

	LeverBox = CreateDefaultSubobject<ULeverBoxComponent>("LeverBox");
	LeverBox->SetupAttachment(Lever);
	LeverBox->SetCollisionResponseToChannel(INTERACT, ECR_Block);
}

void AHydraulicPump::ChangeState()
{
	TArray<FString> Sockets;
	SocketOutputs.GetKeys(Sockets);
	float Pressure = LeverBox->GetStateIndex() ? DefaultPressure : 0;
	for (FString SocketName : Sockets) {
		SetOutputPressure(SocketName, Pressure);
		BFS(SocketName);
	}
}
