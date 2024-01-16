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
	FRelatedActorData LastData;

	while (!Queue.IsEmpty()) {
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);
		FBoardActorOutput* CurrentSocketOutput = CurrentActorData.RelatedActor->SocketOutputs.Find(CurrentActorData.RelatedActorSocket);
		if (!CurrentSocketOutput) continue;
		if (CurrentSocketOutput->bOutMode) {
			if (LastData.IsValid() && CurrentSocketOutput->Pressure == 0.f) {
				FBoardActorOutput* LastSocketOutput = LastData.RelatedActor->SocketOutputs.Find(LastData.RelatedActorSocket);
				if (LastSocketOutput && LastSocketOutput->bOutMode) {
					CurrentActorData.RelatedActor->SetInputPressure(CurrentActorData.RelatedActorSocket, 0.f);
					Queue.Enqueue(CurrentActorData);
					continue;
				}
			}
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
			LastData = CurrentActorData;
		}
		else {
			if (LastData.IsValid() && CurrentSocketOutput->Pressure == 0.f) {
				FBoardActorOutput* LastSocketOutput = LastData.RelatedActor->SocketOutputs.Find(LastData.RelatedActorSocket);
				if (LastSocketOutput && !LastSocketOutput->bOutMode) {
					CurrentActorData.RelatedActor->SetOutputPressure(CurrentActorData.RelatedActorSocket, 0.f);
					Queue.Enqueue(CurrentActorData);
					continue;
				}
			}
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
						LastData = CurrentActorData;
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
	CurrentScheme = SocketRelationsSchemes[0];
	CheckPressure();
}

void AHydraulicPiston::OnSetInputPressureAfter(FString SocketName, float Pressure)
{
	FBoardActorOutput* SocketOutput = SocketOutputs.Find(SocketName);
	if (SocketOutput) {
		if (!SocketOutput->bOutMode && Pressure > 0.f) {
			CurrentScheme = SocketRelationsSchemes[0];
			if (CurrentScheme.SocketRelationsScheme.Num()) {
				FSocketRelations* SocketRelations = CurrentScheme.SocketRelationsScheme.Find(SocketName);
				if (SocketRelations && SocketRelations->SocketRelations.Num()) {
					FBoardActorOutput* SocketRelationOutput = SocketOutputs.Find(SocketRelations->SocketRelations[0]);
					if (SocketRelationOutput) {
						if (SocketRelationOutput->Pressure == 0.f) {
							SocketRelationOutput->bOutMode = true;
							if (FindTank(SocketRelations->SocketRelations[0])) {
								CurrentScheme = SocketRelationsSchemes[0];
								MovePistonIfPossible();
								SocketBroadcast();
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

void AHydraulicPiston::MovePistonIfPossible()
{
	FBoardActorOutput* ASocket = SocketOutputs.Find("A");
	FBoardActorOutput* BSocket = SocketOutputs.Find("B");
	if (bInStart) {
		if ((!ASocket->bOutMode && ASocket->Pressure > 0.f) && (BSocket->bOutMode && BSocket->Pressure == 0.f)) {
			FromLocation = InitialLocation;
			ToLocation = EndLocation;
			bIsTranslationActive = true;
		}
	}
	else {
		if ((!BSocket->bOutMode && BSocket->Pressure > 0.f) && (ASocket->bOutMode && ASocket->Pressure == 0.f)) {
			FromLocation = EndLocation;
			ToLocation = InitialLocation;
			bIsTranslationActive = true;
		}
	}
}

AHydraulicPiston::AHydraulicPiston()
{
	PrimaryActorTick.bCanEverTick = true;
	PistonRod = CreateDefaultSubobject<UStaticMeshComponent>("PistonRod");
	PistonRod->SetupAttachment(RootComponent);
	PistonRod->SetCollisionResponseToChannel(INTERACT, ECollisionResponse::ECR_Block);

	CreateSchemes();
	CurrentScheme = SocketRelationsSchemes[0];
}

void AHydraulicPiston::Tick(float DeltaTime)
{
	if (bIsTranslationActive ) {
		TimeElapsed += DeltaTime;

		float Alpha = FMath::Clamp(TimeElapsed * InterpSpeed / TranslationDuration, 0.f, 1.f);
		FVector NewLocation = FMath::Lerp(FromLocation, ToLocation, Alpha);
		PistonRod->SetRelativeLocation(NewLocation);
		if (TimeElapsed >= (TranslationDuration / InterpSpeed)) {
			bIsTranslationActive = false;
			TimeElapsed = 0.0f;
			bInStart = !bInStart;
		}
	}
}

void AHydraulicPiston::OnSetOutputPressureAfter(FString SocketName, float Pressure)
{
	CurrentScheme = SocketRelationsSchemes[0];
}
