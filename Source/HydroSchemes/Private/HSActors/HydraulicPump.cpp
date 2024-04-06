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
	UE_LOG(LogTemp, Warning, TEXT("Change state for %s"), *GetName());
	for (FString SocketName : Sockets) {
		UE_LOG(LogTemp, Warning, TEXT("SocketName - %s"), *SocketName);
		SetOutputPressure(SocketName, Pressure);
		SchemeUtil::ScanScheme(this, SocketName);
	}
}
