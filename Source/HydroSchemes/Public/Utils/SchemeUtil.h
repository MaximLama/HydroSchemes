// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FRelatedActorData;
struct FBoardActorOutput;
class ABoardSchemeActor;

typedef TSet<FRelatedActorData> TScanVisited;
typedef TQueue<FRelatedActorData> TScanQueue;

/**
 * 
 */
class HYDROSCHEMES_API SchemeUtil
{
public:
	static void ScanScheme(ABoardSchemeActor* StartElement, FString StartSocketName);
private:
	static void SpreadPressure(TScanVisited& Visited, TScanQueue& Queue);
	static void SpreadPressureOutside(TScanVisited& Visited, TScanQueue& Queue, FBoardActorOutput* SocketOutput);
	static void SpreadPressureInside(TScanVisited& Visited, TScanQueue& Queue, FRelatedActorData& RelatedActorData);
	static FRelatedActorData FindSocketWithPressure(const FRelatedActorData& StartRelatedData);
	static FRelatedActorData FindInputActorDataWithPressure(ABoardSchemeActor* CurrentActor, FString CurrentSocketName, TScanVisited& Visited, TScanQueue& Queue, bool bCheckOutput = false);
	static FRelatedActorData FindOutputActorDataWithPressure(ABoardSchemeActor* TargetActor, FString TargetSocketName, TScanVisited& Visited, TScanQueue& Queue, bool bCheckOutput = false);
	static FRelatedActorData FindSocketWithPressureQueueCycle(TScanQueue& Queue, TScanVisited& Visited, bool bCheckOutput = false);
};
