// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicDistributorScheme574.h"
#include "FunctionalComponents/LeverBoxComponent.h"

void AHydraulicDistributorScheme574::Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("T");
	FSocketRelations BRelations;
	BRelations.SocketRelations.Add("P");
	FSocketRelations PRelations;
	PRelations.SocketRelations.Add("B");
	FSocketRelations TRelations;
	TRelations.SocketRelations.Add("A");
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("A"), ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("B"), BRelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("P"), PRelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("T"), TRelations);
}

void AHydraulicDistributorScheme574::Create2Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("P");
	FSocketRelations BRelations;
	BRelations.SocketRelations.Add("T");
	FSocketRelations PRelations;
	PRelations.SocketRelations.Add("A");
	FSocketRelations TRelations;
	TRelations.SocketRelations.Add("B");
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("A"), ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("B"), BRelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("P"), PRelations);
	SocketRelationsScheme.SocketRelationsScheme.Add(FString("T"), TRelations);
}

void AHydraulicDistributorScheme574::CreateSchemes()
{
	FSocketRelationsScheme SocketRelationsScheme1;
	Create1Scheme(SocketRelationsScheme1);
	FSocketRelationsScheme SocketRelationsScheme2;
	Create2Scheme(SocketRelationsScheme2);
	SocketRelationsSchemes.Add(SocketRelationsScheme1);
	SocketRelationsSchemes.Add(SocketRelationsScheme2);
}

AHydraulicDistributorScheme574::AHydraulicDistributorScheme574()
{
	CreateSchemes();
	CurrentScheme = SocketRelationsSchemes[0];
}

void AHydraulicDistributorScheme574::ChangeState()
{
	CurrentScheme = SocketRelationsSchemes[LeverBox->GetStateIndex()];
	CheckPressure();
}