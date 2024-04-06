// HydroSchemes Simulator. All rights reserved.


#include "HSActors/BoardSchemeActor.h"
#include "HSActors/BoardPart.h"
#include "Player/HUBaseCharacter.h"
#include "Interfaces/InteractableInterface.h"
#include "HSActors/Board.h"
#include "HSActors/HydraulicPump.h"
#include "Utils/SchemeUtil.h"

#define INTERACT ECC_GameTraceChannel1
#define BOARD_ACTOR_CHANNEL ECC_GameTraceChannel3

bool ABoardSchemeActor::IsCurrentSchemeEmpty() const
{
	return !(SocketRelationsSchemes.Num() && CurrentScheme.SocketRelationsScheme.Num());
}

const FSocketRelations* ABoardSchemeActor::GetCurrentSocketRelations(FString SocketName) const
{
	return CurrentScheme.SocketRelationsScheme.Find(SocketName);
}

ABoardSchemeActor::ABoardSchemeActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionResponseToChannel(INTERACT, ECollisionResponse::ECR_Block);
}

void ABoardSchemeActor::InitSize(float CellSize)
{
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	FBox CombinedBounds = FBox();
	CombinedBounds = CombinedBounds.MoveTo(GetActorLocation());

	for (UStaticMeshComponent* MeshComponent : MeshComponents) {
		if (MeshComponent) {
			FBox ComponentBounds = MeshComponent->Bounds.GetBox();
			CombinedBounds += ComponentBounds;
		}
	}
	FVector ActorBounds = CombinedBounds.GetSize();
	WidthInCell = FMath::CeilToInt(ActorBounds.Y / CellSize);
	HeightInCell = FMath::CeilToInt(ActorBounds.X / CellSize);
}

void ABoardSchemeActor::SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse)
{
	StaticMeshComponent->SetCollisionResponseToChannel(CollisionChannel, CollisionResponse);
}

void ABoardSchemeActor::SetRCT()
{
	StaticMeshComponent->SetRenderCustomDepth(true);
}

void ABoardSchemeActor::ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult)
{
	auto BoardPart = Cast<ABoardPart>(HitResult.GetActor());
	if (!Character->Holo || Character->Holo->IsActorBeingDestroyed()) {
		Character->Holo = CreateHolo(this, Character->HoloMaterial);
		if (Character->Holo) {
			Character->Holo->AttachToActor(BoardPart, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BoardPart->SlotSocketName);
			Character->SetHoloState(EHoloState::Correct);
			Character->CheckHoloState(BoardPart);
		}
	}
	if (Character->Holo && !Character->Holo->IsActorBeingDestroyed()) {
		AActor* AttachActor = Character->Holo->GetAttachParentActor();
		if ((!AttachActor) || (AttachActor != BoardPart)) {
			if (AttachActor) Character->Holo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Character->Holo->AttachToActor(BoardPart, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BoardPart->SlotSocketName);
			Character->CheckHoloState(BoardPart);
		}
	}
}

ASchemeActor* ABoardSchemeActor::CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial)
{
	if (!GetWorld()) return nullptr;
	auto Holo = GetWorld()->SpawnActor<ABoardSchemeActor>(PlacingElement->GetClass());
	if (!Holo) return nullptr;
	Holo->SetResponseToChannel(INTERACT, ECR_Ignore);
	Holo->SetRCT();
	TWeakObjectPtr<UMaterialInstanceDynamic> HoloDynMatInst = UMaterialInstanceDynamic::Create(HoloMaterial, Holo);
	Holo->SetHoloMaterialInstance(HoloDynMatInst);
	return Holo;
}

TArray<FName> ABoardSchemeActor::GetAllSocketsForCable()
{
	TArray<FName> SocketOutputsArr;
	for (const TPair<FString, FBoardActorOutput>& PairSocket : SocketOutputs)
	{
		SocketOutputsArr.Add(PairSocket.Value.OutputName);
	}
	return SocketOutputsArr;
}

FVector ABoardSchemeActor::GetSocketLocationByName(FName SocketName)
{
	return StaticMeshComponent->GetSocketLocation(SocketName);
}

UStaticMeshComponent* ABoardSchemeActor::GetCableSocketComponent(const FName& SocketName) const
{

	if (StaticMeshComponent->DoesSocketExist(SocketName)) {
		return StaticMeshComponent;
	}
	return nullptr;
}

bool ABoardSchemeActor::CanPlaceHolo(FName SocketName)
{
	for (const TPair<FString, FBoardActorOutput>& Pair : SocketOutputs) {
		if (Pair.Value.OutputName == SocketName) {
			if (Pair.Value.bIsEmployed) return false;
			break;
		}
	}
	return true;
}

bool ABoardSchemeActor::IsSocketOutputExist(FName SocketName)
{
	if (SocketName == NAME_None) return false;
	for (const TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		if (SocketOutput.Value.OutputName == SocketName) return true;
	}
	return false;
}

bool ABoardSchemeActor::IsSocketOutputEmployed(FName SocketName)
{
	for (const TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		if (SocketOutput.Value.OutputName == SocketName) {
			return SocketOutput.Value.bIsEmployed;
		}
	}
	return false;
}

void ABoardSchemeActor::PlaceElement(AHUBaseCharacter* Character)
{
	auto HidroSchemeElement = GetWorld()->SpawnActor<ASchemeActor>(Character->PlacingElement->GetClass());
	auto AttachBoardPart = Cast<ABoardPart>(Character->Holo->GetAttachParentActor());
	HidroSchemeElement->AttachToActor(AttachBoardPart, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBoardPart->SlotSocketName);
	HidroSchemeElement->bIsOnBoard = true;
	HidroSchemeElement->bIsPickUpAble = false;
	if (auto BoardHidroSchemeElement = Cast<ABoardSchemeActor>(HidroSchemeElement)) {
		BoardHidroSchemeElement->SetResponseToChannel(BOARD_ACTOR_CHANNEL, ECR_Block);
		auto Board = Cast<ABoard>(AttachBoardPart->GetAttachParentActor());
		if (Board) {
			int32 Row = -1;
			int32 Column = -1;
			if (Board->CanPlaceHolo(BoardHidroSchemeElement, Row, Column) && Row != -1 && Column != -1) {
				BoardHidroSchemeElement->InitSize(Board->GetBoardPartSize().X);
				Board->EmployBoardParts(Row, Column, BoardHidroSchemeElement->GetWidth(), BoardHidroSchemeElement->GetHeight());
			}
		}
	}
	TArray<UActorComponent*> InteractComponents = HidroSchemeElement->GetComponentsByInterface(UInteractableInterface::StaticClass());
	for (UActorComponent* InteractComponent : InteractComponents) {
		IInteractableInterface* Comp = Cast<IInteractableInterface>(InteractComponent);
		Comp->SetIsInteractable(true);
	}
	Character->DestroyHolo();
}

void ABoardSchemeActor::EmploySocketOutputByName(FName SocketName)
{
	for (TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		if (SocketOutput.Value.OutputName == SocketName) {
			SocketOutput.Value.bIsEmployed = true;
			return;
		}
	}
}

void ABoardSchemeActor::ReleaseSocketOutputByName(FName SocketName)
{
	for (TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		if (SocketOutput.Value.OutputName == SocketName) {
			SocketOutput.Value.bIsEmployed = false;
			return;
		}
	}
}

void ABoardSchemeActor::SetOutputPressure(FString SocketName, float Pressure)
{
	UE_LOG(LogTemp, Warning, TEXT("Start SetOutputPressure for %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("SocketName - %s"), *SocketName);
	UE_LOG(LogTemp, Warning, TEXT("Pressure - %f"), Pressure);
	auto Socket = SocketOutputs.Find(SocketName);
	Socket->Pressure = Pressure;
	Socket->bOutMode = true;
	OnSetOutputPressureAfter(SocketName, Pressure);
	UE_LOG(LogTemp, Warning, TEXT("End SetOutputPressure for %s"), *GetName());
	//if (Socket->RelatedActorData.RelatedActor && !Socket->RelatedActorData.RelatedActorSocket.IsEmpty()) {
	//	Socket->RelatedActorData.RelatedActor->SetInputPressure(Socket->RelatedActorData.RelatedActorSocket, Pressure);
	//}
}

void ABoardSchemeActor::SetInputPressure(FString SocketName, float Pressure)
{
	UE_LOG(LogTemp, Warning, TEXT("Start SetInputPressure for %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("SocketName - %s"), *SocketName);
	UE_LOG(LogTemp, Warning, TEXT("Pressure - %f"), Pressure);
	auto Socket = SocketOutputs.Find(SocketName);
	Socket->Pressure = Pressure;
	Socket->bOutMode = false;
	OnSetInputPressureAfter(SocketName, Pressure);
	UE_LOG(LogTemp, Warning, TEXT("End SetInputPressure for %s"), *GetName());
}

void ABoardSchemeActor::OnSetOutputPressureAfter(FString SocketName, float Pressure)
{
}

void ABoardSchemeActor::OnSetInputPressureAfter(FString SocketName, float Pressure)
{
}

void ABoardSchemeActor::SetSocketRelatedActor(FString ThisSocketName, ABoardSchemeActor* RelatedActor, FString RelatedSocketName)
{
	FBoardActorOutput* Socket = SocketOutputs.Find(ThisSocketName);
	Socket->RelatedActorData.RelatedActor = RelatedActor;
	Socket->RelatedActorData.RelatedActorSocket = RelatedSocketName;
}

void ABoardSchemeActor::CheckPressure()
{
	// in > 0 - ничего не делать
	// out > 0 - нужно искать источник давления для перерасчета
	// in = 0 - ничего не делать
	// out = 0 - ничего не делать

	// in = 0 in = 0 - запустить bfs так как это может быть новый элемент
	// in > 0 in > 0 - ничего не делать 
	// in > 0 in = 0 - запустить bfs с 1 in
	// out > 0 out > 0 - найти источник питания для первого
	// out > 0 out = 0 - найти источник давления
	// in > 0 out > 0 - ничего не делать
	// in > 0 out = 0 - bfs для in
	// in = 0 out > 0 - bfs для out
	// in = 0 out = 0 - ничего не делать

	TSet<FString> Visited;
	UE_LOG(LogTemp, Warning, TEXT("Start CheckPressure for %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("CurrentScheme - %s"), *CurrentScheme.ToString());
	for (TPair<FString, FSocketRelations> SocketRelation : CurrentScheme.SocketRelationsScheme) {
		UE_LOG(LogTemp, Warning, TEXT("Iteration: SocketRelation: Key - %s, Value - %s"), *SocketRelation.Key, *SocketRelation.Value.ToString());
		if (Visited.Contains(SocketRelation.Key)) continue;
		Visited.Add(SocketRelation.Key);
		
		FBoardActorOutput* FirstSocketOutput = SocketOutputs.Find(SocketRelation.Key);
		if (!FirstSocketOutput) continue;
		for (FString SocketName : SocketRelation.Value.SocketRelations) {
			UE_LOG(LogTemp, Warning, TEXT("Subiteration - %s"), *SocketName);
			Visited.Add(SocketName);
			FBoardActorOutput* SecondSocketOutput = SocketOutputs.Find(SocketName);
			UE_LOG(LogTemp, Warning, TEXT("FirstSocketOutput: Mode - %i, Pressure - %f; SecondSocketOutput: Mode - %i, Pressure - %f"), FirstSocketOutput->bOutMode, FirstSocketOutput->Pressure, SecondSocketOutput->bOutMode, SecondSocketOutput->Pressure);
			// in = 0 in = 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.0f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.0f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				if (FirstSocketOutput->Pressure == 0.f) {
					SetOutputPressure(SocketName, 0.f);
					SchemeUtil::ScanScheme(this, SocketName);
				}
				continue;
			}
			// in > 0 in = 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				continue;
			}
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SchemeUtil::ScanScheme(this, SocketName);
				continue;
			}

			//out > 0 out > 0
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				if (FirstSocketOutput->Pressure == 0.f) {
					SetOutputPressure(SocketName, 0.f);
					SchemeUtil::ScanScheme(this, SocketName);
				}
				continue;
			}

			//out > 0 out = 0
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				continue;
			}
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketName, 0.f);
				SchemeUtil::ScanScheme(this, SocketName);
				continue;
			}

			//in > 0 out = 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				continue;
			}

			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SchemeUtil::ScanScheme(this, SocketName);
				continue;
			}

			//in = 0 out > 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketName, 0.f);
				SchemeUtil::ScanScheme(this, SocketName);
				continue;
			}

			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				SchemeUtil::ScanScheme(this, SocketRelation.Key);
				continue;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Single CheckPressure for %s"), *GetName());
	for (TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		UE_LOG(LogTemp, Warning, TEXT("Single Iterate: SocketOutput - %s: %s"), *SocketOutput.Key, *SocketOutput.Value.ToString());
		if (Visited.Contains(SocketOutput.Key)) continue;
		if (SocketOutput.Value.bOutMode && SocketOutput.Value.Pressure > 0.f) {
			SetOutputPressure(SocketOutput.Key, 0.f);
			SchemeUtil::ScanScheme(this, SocketOutput.Key);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("End CheckPressure for %s"), *GetName());
}

void ABoardSchemeActor::SocketBroadcast()
{
	for (TPair<FString, FBoardActorOutput>& Socket : SocketOutputs) {
		Socket.Value.Delegate.Broadcast();
	}
}

FString ABoardSchemeActor::PrintSocketOutputs()
{
	FString Result;
	for (TPair<FString, FBoardActorOutput> SocketOutput : SocketOutputs) {
		Result += SocketOutput.Key + ": {\n" + SocketOutput.Value.ToString() + "}\n";
	}
	return Result;
}


FString FRelatedActorData::ToString() {
	FString Result;
	Result += "RelatedActor: ";
	Result += RelatedActor ? RelatedActor->GetName() : "null";
	Result += "\n";
	Result += "RelatedActorSocket: " + RelatedActorSocket;
	return Result;
}

FBoardActorOutput* ABoardSchemeActor::GetSocketInfo(FString SocketName)
{
	return SocketOutputs.Find(SocketName);
}
