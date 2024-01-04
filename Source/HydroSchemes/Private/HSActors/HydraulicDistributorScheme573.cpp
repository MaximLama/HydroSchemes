// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicDistributorScheme573.h"
#include "FunctionalComponents/LeverBoxComponent.h"

void AHydraulicDistributorScheme573::Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("T");
	FSocketRelations TRelations;
	TRelations.SocketRelations.Add("A");
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("A"), ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("T"), TRelations);
}

void AHydraulicDistributorScheme573::Create2Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("P");
	FSocketRelations PRelations;
	PRelations.SocketRelations.Add("A");
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("A"), ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("P"), PRelations);
}

void AHydraulicDistributorScheme573::CreateSchemes()
{
	FSocketRelationsScheme SocketRelationsScheme1;
	Create1Scheme(SocketRelationsScheme1);
	FSocketRelationsScheme SocketRelationsScheme2;
	Create2Scheme(SocketRelationsScheme2);
	SocketRelationsSchemes.Add(SocketRelationsScheme1);
	SocketRelationsSchemes.Add(SocketRelationsScheme2);
}

AHydraulicDistributorScheme573::AHydraulicDistributorScheme573()
{

	CreateSchemes();
	CurrentScheme = SocketRelationsSchemes[0];
}

void AHydraulicDistributorScheme573::ChangeState()
{
	CurrentScheme = SocketRelationsSchemes[LeverBox->GetStateIndex()];
	SocketBroadcast();
	CheckPressure();
}
