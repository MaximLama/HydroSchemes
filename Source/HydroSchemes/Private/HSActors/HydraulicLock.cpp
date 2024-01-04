// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicLock.h"

void AHydraulicLock::Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("B");
	SocketRelationsScheme.SocketRelationsScheme.Add("A", ARelations);
}

void AHydraulicLock::Create2Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("B");
	FSocketRelations BRelations;
	BRelations.SocketRelations.Add("A");
	SocketRelationsScheme.SocketRelationsScheme.Add("A", ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add("B", BRelations);
}

void AHydraulicLock::CreateSchemes()
{
	FSocketRelationsScheme SocketRelationScheme1;
	Create1Scheme(SocketRelationScheme1);
	FSocketRelationsScheme SocketRelationScheme2;
	Create1Scheme(SocketRelationScheme2);
	SocketRelationsSchemes.Add(SocketRelationScheme1);
	SocketRelationsSchemes.Add(SocketRelationScheme2);
}

AHydraulicLock::AHydraulicLock()
{
	CreateSchemes();
	CurrentScheme = SocketRelationsSchemes[0];
}

void AHydraulicLock::OnSetInputPressureAfter(FString SocketName, float Pressure)
{
	if (SocketName == "X") {
		FBoardActorOutput* XSocketOutput = SocketOutputs.Find("X");
		bool isPressureSaved = (XSocketOutput->Pressure == Pressure);
		CurrentScheme = (Pressure > 0) ? SocketRelationsSchemes[1] : SocketRelationsSchemes[0];
		if (!isPressureSaved) {
			CheckPressure();
		}
	}
}
