// HydroSchemes Simulator. All rights reserved.


#include "HSActors/BoardPart.h"
#include "HSActors/Board.h"

DEFINE_LOG_CATEGORY_STATIC(BaseBoardPartLog, All, All)

ABoardPart::ABoardPart()
{
	PrimaryActorTick.bCanEverTick = false;

	//Создание компонентов сцены
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(GetRootComponent());
}

FVector ABoardPart::GetLocalObjectSize()
{
	if (StaticMesh) {
		FVector WorldObjectSize = StaticMesh->Bounds.GetBox().GetSize();
		return WorldObjectSize;
	}
	return FVector();
}

// Called when the game starts or when spawned
void ABoardPart::BeginPlay()
{
	Super::BeginPlay();
	
	//if (!StaticMesh) return;
	//FVector Min = FVector();
	//FVector Max = FVector();
	//StaticMesh->GetLocalBounds(Min, Max);
	//UE_LOG(BaseBoardPartLog, Display, TEXT("MIN: %s"), *Min.ToString());
	//UE_LOG(BaseBoardPartLog, Display, TEXT("MIN: %s"), *Max.ToString());
}

