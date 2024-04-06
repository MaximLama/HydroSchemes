// HydroSchemes Simulator. All rights reserved.


#include "Utils/SchemeUtil.h"
#include "HSActors/BoardSchemeActor.h"
#include "HSActors/HydraulicPump.h"

void SchemeUtil::ScanScheme(ABoardSchemeActor* StartElement, FString StartSocketName)
{
	// Получаем стартовый сокет, с которого начнется поиск в ширину
	const FBoardActorOutput* FirstSocketOutput = StartElement->SocketOutputs.Find(StartSocketName);
	if (!FirstSocketOutput) return;

	if (FirstSocketOutput->Pressure > 0.f && !FirstSocketOutput->bOutMode) {
		StartElement->OnSetInputPressureAfter(StartSocketName, FirstSocketOutput->Pressure);
	}

	//Получаем текущее давление
	float CurrentPressure = FirstSocketOutput->Pressure;

	FRelatedActorData FirstActorData(StartElement, StartSocketName);

	//обход в ширину при наличии давления
	if (CurrentPressure) {
		// Создаем список посещенных сокетов и список сокетов, ожидающих посещения
		TScanVisited Visited;
		TScanQueue Queue;

		//Добавляем в множество и очередь первый сокет
		
		Visited.Add(FirstActorData);
		Queue.Enqueue(FirstActorData);
		//обходим пока есть сокеты, ожидающие посещения
		SpreadPressure(Visited, Queue);
	}
	else {
		FRelatedActorData InputPressureData = FindSocketWithPressure(FirstActorData);
		if (InputPressureData.IsValid()) {
			SchemeUtil::ScanScheme(InputPressureData.RelatedActor, InputPressureData.RelatedActorSocket);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("End BFS"));
}

void SchemeUtil::SpreadPressure(TScanVisited& Visited, TScanQueue& Queue)
{
	while (!Queue.IsEmpty()) {
		//получаем первый сокет из очереди
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);

		//получаем полную информацию по сокету
		FBoardActorOutput* CurrentSocketOutput = CurrentActorData.RelatedActor->GetSocketInfo(CurrentActorData.RelatedActorSocket);
		if (!CurrentSocketOutput) continue;


		CurrentSocketOutput->bOutMode 
			? SpreadPressureOutside(Visited, Queue, CurrentSocketOutput)
			: SpreadPressureInside(Visited, Queue, CurrentActorData);
	}
}

void SchemeUtil::SpreadPressureOutside(TScanVisited& Visited, TScanQueue& Queue, FBoardActorOutput* SocketOutput)
{
	if (!SocketOutput->bOutMode) return;
	if (!SocketOutput->RelatedActorData.IsValid()) return;
	//проверяем, что связанный сокет не был посещен
	if (Visited.Contains(SocketOutput->RelatedActorData)) return;
	Visited.Add(SocketOutput->RelatedActorData);
	
	//нет смысла подавать давление на насос
	AHydraulicPump* Pump = Cast<AHydraulicPump>(SocketOutput->RelatedActorData.RelatedActor);
	if (Pump) return;
	
	//получаем полную информацию по связанному сокету
	FBoardActorOutput* TargetSocketOutput = SocketOutput->RelatedActorData.RelatedActor->GetSocketInfo(SocketOutput->RelatedActorData.RelatedActorSocket);
	if (!TargetSocketOutput) return;
	if (TargetSocketOutput->Pressure > 0.f) return;
	//подаем давление на связанный сокет
	SocketOutput->RelatedActorData.RelatedActor->SetInputPressure(SocketOutput->RelatedActorData.RelatedActorSocket, SocketOutput->Pressure);
	//добавляем новый сокет в очередь
	Queue.Enqueue(SocketOutput->RelatedActorData);
}

void SchemeUtil::SpreadPressureInside(TScanVisited& Visited, TScanQueue& Queue, FRelatedActorData& RelatedActorData)
{
	ABoardSchemeActor* TargetActor = RelatedActorData.RelatedActor;
	//проверяем, есть ли у элемента гидросхемы схемы связей сокетов внутри элемента и, если есть, то
	//проверить, что текущая схема не пустая 
	if (!TargetActor->IsCurrentSchemeEmpty()) {
		//получаем связи с сокетом текущего объекта гидросхемы
		const FSocketRelations* TargetSocketRelations = TargetActor->GetCurrentSocketRelations(RelatedActorData.RelatedActorSocket);
		if (TargetSocketRelations && !TargetSocketRelations->IsEmpty()) {
			//проход по всем связанным сокетам
			for (FString SocketName : TargetSocketRelations->SocketRelations) {

				FRelatedActorData TargetActorData(TargetActor, SocketName);
				// пропустить сокет, если посещали
				if (Visited.Contains(TargetActorData)) continue;
				// добавить сокет в посещенные
				Visited.Add(TargetActorData);
				// получить полную информацию по связанному сокету
				FBoardActorOutput* OutSocket = TargetActor->GetSocketInfo(SocketName);
				// проверяем что связанный сокет существует и не является входным
				if (OutSocket && !OutSocket->Pressure) {
					//подаем добавление и добавляем в очередь
					TargetActor->SetOutputPressure(SocketName, TargetActor->GetSocketInfo(RelatedActorData.RelatedActorSocket)->Pressure);
					Queue.Enqueue(TargetActorData);
				}
			}
		}
	}
}

FRelatedActorData SchemeUtil::FindSocketWithPressure(const FRelatedActorData& StartRelatedData)
{
	TSet<FRelatedActorData> Visited;
	TQueue<FRelatedActorData> Queue;

	Visited.Add(StartRelatedData);

	ABoardSchemeActor* StartActor = StartRelatedData.RelatedActor;
	FBoardActorOutput* StartSocketOutput = StartActor->GetSocketInfo(StartRelatedData.RelatedActorSocket);

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

FRelatedActorData SchemeUtil::FindSocketWithPressureQueueCycle(TScanQueue& Queue, TScanVisited& Visited, bool bCheckOutput)
{
	while (!Queue.IsEmpty()) {
		FRelatedActorData CurrentActorData;
		Queue.Dequeue(CurrentActorData);
		ABoardSchemeActor* CurrentActor = CurrentActorData.RelatedActor;
		FBoardActorOutput* CurrentSocketOutput = CurrentActor->GetSocketInfo(CurrentActorData.RelatedActorSocket);
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

FRelatedActorData SchemeUtil::FindInputActorDataWithPressure(ABoardSchemeActor* CurrentActor, FString CurrentSocketName, TScanVisited& Visited, TScanQueue& Queue, bool bCheckOutput)
{
	if (!CurrentActor->IsCurrentSchemeEmpty()) {
		const FSocketRelations* CurrentSocketRelations = CurrentActor->GetCurrentSocketRelations(CurrentSocketName);
		if (CurrentSocketRelations && !CurrentSocketRelations->IsEmpty()) {
			for (FString SocketName : CurrentSocketRelations->SocketRelations) {
				FRelatedActorData TargetRelatedData(CurrentActor, SocketName);
				if (Visited.Contains(TargetRelatedData)) continue;
				Visited.Add(TargetRelatedData);
				FBoardActorOutput* InSocket = CurrentActor->GetSocketInfo(SocketName);
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

FRelatedActorData SchemeUtil::FindOutputActorDataWithPressure(ABoardSchemeActor* TargetActor, FString TargetSocketName, TScanVisited& Visited, TScanQueue& Queue, bool bCheckOutput)
{
	FBoardActorOutput* TargetSocketOutput = TargetActor->GetSocketInfo(TargetSocketName);
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
