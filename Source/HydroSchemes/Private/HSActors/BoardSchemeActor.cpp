// HydroSchemes Simulator. All rights reserved.


#include "HSActors/BoardSchemeActor.h"
#include "HSActors/BoardPart.h"
#include "Player/HUBaseCharacter.h"
#include "Interfaces/InteractableInterface.h"
#include "HSActors/Board.h"
#include "HSActors/HydraulicPump.h"

#define INTERACT ECC_GameTraceChannel1
#define BOARD_ACTOR_CHANNEL ECC_GameTraceChannel3

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
	auto Socket = SocketOutputs.Find(SocketName);
	Socket->Pressure = Pressure;
	Socket->bOutMode = true;
	//if (Socket->RelatedActorData.RelatedActor && !Socket->RelatedActorData.RelatedActorSocket.IsEmpty()) {
	//	Socket->RelatedActorData.RelatedActor->SetInputPressure(Socket->RelatedActorData.RelatedActorSocket, Pressure);
	//}
}

void ABoardSchemeActor::SetInputPressure(FString SocketName, float Pressure)
{
	auto Socket = SocketOutputs.Find(SocketName);
	Socket->Pressure = Pressure;
	Socket->bOutMode = false;
	OnSetInputPressureAfter(SocketName, Pressure);
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

void ABoardSchemeActor::BFS(FString StartSocketName)
{

	// Получаем стартовый сокет, с которого начнется поиск в ширину
	FBoardActorOutput* FirstSocketOutput = SocketOutputs.Find(StartSocketName);
	if(!FirstSocketOutput) return;
	// Создаем список посещенных сокетов и список сокетов, ожидающих посещения
	TSet<FRelatedActorData> Visited;
	TQueue<FRelatedActorData> Queue;

	//Получаем текущее давление
	float CurrentPressure = FirstSocketOutput->Pressure;

	//Добавляем в множество и очередь первый сокет
	FRelatedActorData FirstActorData(this, StartSocketName);
	Visited.Add(FirstActorData);
	Queue.Enqueue(FirstActorData);

	//обход в ширину при наличии давления
	if (CurrentPressure) {
		//обходим пока есть сокеты, ожидающие посещения
		while (!Queue.IsEmpty()) {
			//получаем первый сокет из очереди
			FRelatedActorData CurrentActorData;
			Queue.Dequeue(CurrentActorData);
			//получаем полную информацию по сокету
			FBoardActorOutput* CurrentSocketOutput = CurrentActorData.RelatedActor->SocketOutputs.Find(CurrentActorData.RelatedActorSocket);
			if (!CurrentSocketOutput) continue;
			//если сокет выходной
			if (CurrentSocketOutput->bOutMode) {
				//проверяем на наличие связанного сокета
				if (!CurrentSocketOutput->RelatedActorData.IsValid()) continue;
				//проверяем, что связанный сокет не был посещен
				if (Visited.Contains(CurrentSocketOutput->RelatedActorData)) continue;
				Visited.Add(CurrentSocketOutput->RelatedActorData);
				AHydraulicPump* Pump = Cast<AHydraulicPump>(CurrentSocketOutput->RelatedActorData.RelatedActor);
				if (Pump) continue;
				//получаем полную информацию по связанному сокету
				FBoardActorOutput* TargetSocketOutput = CurrentSocketOutput->RelatedActorData.RelatedActor->SocketOutputs.Find(CurrentSocketOutput->RelatedActorData.RelatedActorSocket);
				if (!TargetSocketOutput) continue;
				if (TargetSocketOutput->Pressure > 0.f) continue;
				//подаем давление на связанный сокет
				CurrentSocketOutput->RelatedActorData.RelatedActor->SetInputPressure(CurrentSocketOutput->RelatedActorData.RelatedActorSocket, CurrentPressure);
				//добавляем новый сокет в очередь
				Queue.Enqueue(CurrentSocketOutput->RelatedActorData);
			}
			//если сокет входной
			else {
				ABoardSchemeActor* TargetActor = CurrentActorData.RelatedActor;
				
				//проверяем, есть ли у элемента гидросхемы схемы связей сокетов внутри элемента и, если есть, то
				//проверить, что текущая схема не пустая 
				if (TargetActor->SocketRelationsSchemes.Num() && TargetActor->CurrentScheme.SocketRelationsScheme.Num()) {
					//получаем связи с сокетом текущего объекта гидросхемы
					FSocketRelations* TargetSocketRelations = TargetActor->CurrentScheme.SocketRelationsScheme.Find(CurrentActorData.RelatedActorSocket);
					if (TargetSocketRelations && TargetSocketRelations->SocketRelations.Num()) {
						//проход по всем связанным сокетам
						for (FString SocketName : TargetSocketRelations->SocketRelations) {
							FRelatedActorData TargetActorData(TargetActor, SocketName);
							// пропустить сокет, если посещали
							if (Visited.Contains(TargetActorData)) continue;
							// добавить сокет в посещенные
							Visited.Add(TargetActorData);
							// получить полную информацию по связанному сокету
							FBoardActorOutput* OutSocket = TargetActor->SocketOutputs.Find(SocketName);
							// проверяем что связанный сокет существует и не является входным
							if (OutSocket && !OutSocket->Pressure) {
								//подаем добавление и добавляем в очередь
								TargetActor->SetOutputPressure(SocketName, CurrentPressure);
								Queue.Enqueue(TargetActorData);
							}
						}
					}
				}
			}
		}
	}
	else {
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);
		FRelatedActorData InputPressureData = FindSocketWithPressure(CurrentActorData);
		if (InputPressureData.IsValid()) {
			InputPressureData.RelatedActor->BFS(InputPressureData.RelatedActorSocket);
		}
	}
}

FRelatedActorData ABoardSchemeActor::FindSocketWithPressure(const FRelatedActorData& StartRelatedData)
{
	TSet<FRelatedActorData> Visited;
	TQueue<FRelatedActorData> Queue;

	Visited.Add(StartRelatedData);
	
	ABoardSchemeActor* StartActor = StartRelatedData.RelatedActor;
	FBoardActorOutput* StartSocketOutput = StartActor->SocketOutputs.Find(StartRelatedData.RelatedActorSocket);
	if (StartSocketOutput) {
		if (StartSocketOutput->bOutMode) {
			FRelatedActorData RequiredActorData = FindInputActorDataWithPressure(StartActor, StartRelatedData.RelatedActorSocket, Visited, Queue);
			if (RequiredActorData.IsValid()) return RequiredActorData;
			RequiredActorData = FindSocketWithPressureQueueCycle(Queue, Visited);
			if (RequiredActorData.IsValid()) return RequiredActorData;
			if (StartSocketOutput->RelatedActorData.IsValid()) {
				ABoardSchemeActor* TargetActor = StartSocketOutput->RelatedActorData.RelatedActor;
				RequiredActorData = FindOutputActorDataWithPressure(StartSocketOutput->RelatedActorData.RelatedActor, StartSocketOutput->RelatedActorData.RelatedActorSocket, Visited, Queue, true);
				if (RequiredActorData.IsValid()) return RequiredActorData;
				RequiredActorData = FindSocketWithPressureQueueCycle(Queue, Visited, true);
				if (RequiredActorData.IsValid()) return RequiredActorData;
			}
		}
	}
	return FRelatedActorData();
}

FRelatedActorData ABoardSchemeActor::FindInputActorDataWithPressure(ABoardSchemeActor* CurrentActor, FString CurrentSocketName, TSet<FRelatedActorData>& Visited, TQueue<FRelatedActorData>& Queue, bool bCheckOutput)
{
	if (CurrentActor->SocketRelationsSchemes.Num() && CurrentActor->CurrentScheme.SocketRelationsScheme.Num()) {
		FSocketRelations* CurrentSocketRelations = CurrentActor->CurrentScheme.SocketRelationsScheme.Find(CurrentSocketName);
		if (CurrentSocketRelations && CurrentSocketRelations->SocketRelations.Num()) {
			for (FString SocketName : CurrentSocketRelations->SocketRelations) {
				FRelatedActorData TargetRelatedData(CurrentActor, SocketName);
				if (Visited.Contains(TargetRelatedData)) continue;
				Visited.Add(TargetRelatedData);
				FBoardActorOutput* InSocket = CurrentActor->SocketOutputs.Find(SocketName);
				if (InSocket) {
					if (!InSocket->bOutMode) {
						if (InSocket->Pressure) return FRelatedActorData(CurrentActor, SocketName);
						Queue.Enqueue(TargetRelatedData);
					}
					else {
						if (bCheckOutput) {
							if (InSocket->Pressure > 0.f) {
								CurrentActor->SetInputPressure(SocketName, 0.f);
								Queue.Enqueue(TargetRelatedData);
							}
						}
					}
				}
			}
		}
	}
	return FRelatedActorData();
}

FRelatedActorData ABoardSchemeActor::FindOutputActorDataWithPressure(ABoardSchemeActor* TargetActor, FString TargetSocketName, TSet<FRelatedActorData>& Visited, TQueue<FRelatedActorData>& Queue, bool bCheckOutput)
{
	FBoardActorOutput* TargetSocketOutput = TargetActor->SocketOutputs.Find(TargetSocketName);
	if (TargetSocketOutput) {
		FRelatedActorData TargetActorData(TargetActor, TargetSocketName);
		if (Visited.Contains(TargetActorData)) return FRelatedActorData();
		Visited.Add(TargetActorData);
		if (TargetSocketOutput->bOutMode) {
			if (TargetSocketOutput->Pressure > 0.f) return TargetActorData;
			Queue.Enqueue(TargetActorData);
		}
		if (bCheckOutput) {
			if (!TargetSocketOutput->bOutMode) {
				if (TargetSocketOutput->Pressure > 0.f) {
					TargetActor->SetOutputPressure(TargetSocketName, 0.f);
					Queue.Enqueue(TargetActorData);
				}
			}
		}
	}
	return FRelatedActorData();
}

FRelatedActorData ABoardSchemeActor::FindSocketWithPressureQueueCycle(TQueue<FRelatedActorData>& Queue, TSet<FRelatedActorData>& Visited, bool bCheckOutput)
{
	while (!Queue.IsEmpty()) {
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);
		ABoardSchemeActor* CurrentActor = CurrentActorData.RelatedActor;
		FBoardActorOutput* CurrentSocketOutput = CurrentActor->SocketOutputs.Find(CurrentActorData.RelatedActorSocket);
		if (CurrentSocketOutput) {
			if (CurrentSocketOutput->bOutMode) {
				FRelatedActorData RequiredActorData = FindInputActorDataWithPressure(CurrentActor, CurrentActorData.RelatedActorSocket, Visited, Queue, bCheckOutput);
				if (RequiredActorData.IsValid()) return RequiredActorData;
			}
			else {
				if (CurrentSocketOutput->RelatedActorData.IsValid()) {
					ABoardSchemeActor* TargetActor = CurrentSocketOutput->RelatedActorData.RelatedActor;
					FRelatedActorData RequiredActorData = FindOutputActorDataWithPressure(CurrentSocketOutput->RelatedActorData.RelatedActor, CurrentSocketOutput->RelatedActorData.RelatedActorSocket, Visited, Queue, bCheckOutput);
					if (RequiredActorData.IsValid()) return RequiredActorData;
				}
			}
		}
	}
	return FRelatedActorData();
}

void ABoardSchemeActor::CheckPressure()
{
	// in > 0 - ничего не делать
	// out > 0 - нужно искать источник давления для перерасчета
	// in = 0 - ничего не делать
	// out = 0 - ничего не делать

	// in > 0 in > 0 - ничего не делать 
	// in > 0 in = 0 - запустить bfs с 1 in
	// out > 0 out > 0 - найти источник питания для первого
	// out > 0 out = 0 - найти источник давления
	// in > 0 out > 0 - ничего не делать
	// in > 0 out = 0 - bfs для in
	// in = 0 out > 0 - bfs для out
	// in = 0 out = 0 - ничего не делать

	TSet<FString> Visited;
	for (TPair<FString, FSocketRelations> SocketRelation : CurrentScheme.SocketRelationsScheme) {
		if (Visited.Contains(SocketRelation.Key)) continue;
		Visited.Add(SocketRelation.Key);
		FBoardActorOutput* FirstSocketOutput = SocketOutputs.Find(SocketRelation.Key);
		if (!FirstSocketOutput) continue;
		for (FString SocketName : SocketRelation.Value.SocketRelations) {
			Visited.Add(SocketName);
			FBoardActorOutput* SecondSocketOutput = SocketOutputs.Find(SocketName);
			// in > 0 in = 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				BFS(SocketRelation.Key);
				continue;
			}
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				BFS(SocketName);
				continue;
			}

			//out > 0 out > 0
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				BFS(SocketRelation.Key);
				if (FirstSocketOutput->Pressure == 0.f) {
					SetOutputPressure(SocketName, 0.f);
					BFS(SocketName);
				}
				continue;
			}

			//out > 0 out = 0
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				BFS(SocketRelation.Key);
				continue;
			}
			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketName, 0.f);
				BFS(SocketName);
				continue;
			}

			//in > 0 out = 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				BFS(SocketRelation.Key);
				continue;
			}

			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				BFS(SocketName);
				continue;
			}

			//in = 0 out > 0
			if ((!FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure == 0.f) && (SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure > 0.f)) {
				SetOutputPressure(SocketName, 0.f);
				BFS(SocketName);
				continue;
			}

			if ((FirstSocketOutput->bOutMode && FirstSocketOutput->Pressure > 0.f) && (!SecondSocketOutput->bOutMode && SecondSocketOutput->Pressure == 0.f)) {
				SetOutputPressure(SocketRelation.Key, 0.f);
				BFS(SocketRelation.Key);
				continue;
			}
		}
	}
	for (TPair<FString, FBoardActorOutput>& SocketOutput : SocketOutputs) {
		if (Visited.Contains(SocketOutput.Key)) continue;
		if (SocketOutput.Value.bOutMode && SocketOutput.Value.Pressure > 0.f) {
			SetOutputPressure(SocketOutput.Key, 0.f);
			BFS(SocketOutput.Key);
		}
	}
}

void ABoardSchemeActor::SocketBroadcast()
{
	for (TPair<FString, FBoardActorOutput>& Socket : SocketOutputs) {
		Socket.Value.Delegate.Broadcast();
	}
}
