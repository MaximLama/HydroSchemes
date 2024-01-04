// HydroSchemes Simulator. All rights reserved.


#include "HSActors/HydraulicPiston.h"
#include "HSActors/HydraulicTank.h"

#define INTERACT ECC_GameTraceChannel1

void AHydraulicPiston::Create1Scheme(FSocketRelationsScheme& SocketRelationsScheme)
{
	FSocketRelations ARelations;
	ARelations.SocketRelations.Add("B");
	FSocketRelations BRelations;
	BRelations.SocketRelations.Add("A");
	SocketRelationsScheme.SocketRelationsScheme.Add("A", ARelations);
	SocketRelationsScheme.SocketRelationsScheme.Add("B", BRelations);
}

void AHydraulicPiston::CreateSchemes()
{
	FSocketRelationsScheme SocketRelationScheme1;
	Create1Scheme(SocketRelationScheme1);
	FSocketRelationsScheme SocketRelationScheme2;
	SocketRelationsSchemes.Add(SocketRelationScheme1);
	SocketRelationsSchemes.Add(SocketRelationScheme2);
}

bool AHydraulicPiston::FindTank(FString SocketName)
{
	FBoardActorOutput* SocketOutput = SocketOutputs.Find(SocketName);
	TQueue<FRelatedActorData> Queue;
	TSet<FRelatedActorData> BlockedElements;
	TSet<FRelatedActorData> Visited;

	Queue.Enqueue(FRelatedActorData(this, SocketName));
	Visited.Add(FRelatedActorData(this, SocketName));

	FRelatedActorData LastInputData;

	while (!Queue.IsEmpty()) {
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);
		FBoardActorOutput* CurrentSocketOutput = CurrentActorData.RelatedActor->SocketOutputs.Find(CurrentActorData.RelatedActorSocket);
		if (!CurrentSocketOutput) continue;
		if (CurrentSocketOutput->bOutMode) {
			if (!CurrentSocketOutput->RelatedActorData.IsValid()) {
				if (LastInputData.IsValid()) {
					BlockedElements.Add(LastInputData);
				}
				continue;
			}
			if (Visited.Contains(CurrentSocketOutput->RelatedActorData)) continue;
			Visited.Add(CurrentSocketOutput->RelatedActorData);
			AHydraulicTank* Tank = Cast<AHydraulicTank>(CurrentSocketOutput->RelatedActorData.RelatedActor);
			if (Tank) return true;
			Queue.Enqueue(CurrentSocketOutput->RelatedActorData);
		}
		else {
			LastInputData = CurrentActorData;
			ABoardSchemeActor* TargetActor = CurrentActorData.RelatedActor;
			if (TargetActor->SocketRelationsSchemes.Num() && TargetActor->CurrentScheme.SocketRelationsScheme.Num()) {
				FSocketRelations* TargetSocketRelations = TargetActor->CurrentScheme.SocketRelationsScheme.Find(CurrentActorData.RelatedActorSocket);
				if (TargetSocketRelations && TargetSocketRelations->SocketRelations.Num()) {
					for (FString SocketRelation : TargetSocketRelations->SocketRelations) {
						FRelatedActorData TargetActorData(TargetActor, SocketRelation);
						if (Visited.Contains(TargetActorData)) continue;
						Visited.Add(TargetActorData);
						Queue.Enqueue(TargetActorData);
					}
				}
				else {
					BlockedElements.Add(LastInputData);
				}
			}
			else {
				BlockedElements.Add(LastInputData);
			}
		}
	}
	for (FRelatedActorData ActorData : BlockedElements) {
		FBoardActorOutput* BlockedSocketOutput = ActorData.RelatedActor->SocketOutputs.Find(ActorData.RelatedActorSocket);
		if (BlockedSocketOutput) {
			BlockedSocketOutput->Delegate.AddUObject(this, &AHydraulicPiston::UpdateState);
		}
	}
	return false;
}

void AHydraulicPiston::UpdateState()
{
	UE_LOG(LogTemp, Warning, TEXT("Work"));
}

void AHydraulicPiston::OnSetInputPressureAfter(FString SocketName, float Pressure)
{
	UE_LOG(LogTemp, Warning, TEXT("IN"));
	FBoardActorOutput* SocketOutput = SocketOutputs.Find(SocketName);
	if (SocketOutput) {
		if (!SocketOutput->bOutMode && Pressure > 0.f) {
			CurrentScheme = SocketRelationsSchemes[0];
			if (CurrentScheme.SocketRelationsScheme.Num()) {
				FSocketRelations* SocketRelations = CurrentScheme.SocketRelationsScheme.Find(SocketName);
				if (SocketRelations && SocketRelations->SocketRelations.Num()) {
					UE_LOG(LogTemp, Warning, TEXT("socket name - %s"), *SocketRelations->SocketRelations[0]);
					FBoardActorOutput* SocketRelationOutput = SocketOutputs.Find(SocketRelations->SocketRelations[0]);
					if (SocketRelationOutput) {
						if ((!SocketRelationOutput->bOutMode) && (SocketRelationOutput->Pressure == 0.f)) {
							SocketRelationOutput->bOutMode = true;
							UE_LOG(LogTemp, Warning, TEXT("Yes"));
							if (FindTank(SocketRelations->SocketRelations[0])) {
								CurrentScheme = SocketRelationsSchemes[0];
							}
							else {
								CurrentScheme = SocketRelationsSchemes[1];
							}
						}
						else {
							CurrentScheme = SocketRelationsSchemes[1];
						}
					}
				}
			}
		}
		else {
			CurrentScheme = SocketRelationsSchemes[1];
		}
	}
}

AHydraulicPiston::AHydraulicPiston()
{
	PistonRod = CreateDefaultSubobject<UStaticMeshComponent>("PistonRod");
	PistonRod->SetupAttachment(RootComponent);
	PistonRod->SetCollisionResponseToChannel(INTERACT, ECollisionResponse::ECR_Block);

	CreateSchemes();
	CurrentScheme = SocketRelationsSchemes[0];
}
