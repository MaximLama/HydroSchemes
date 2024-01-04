// HydroSchemes Simulator. All rights reserved.


#include "HSActors/Plus.h"
#include "HSActors/Board.h"
#include "FunctionalComponents/PlusButtonStrategyComponent.h"
#include "HSActors/PlusSphereComponent.h"
#include "Engine/Engine.h"

#define INTERACT ECC_GameTraceChannel1

// Sets default values
APlus::APlus()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	SphereComponent = CreateDefaultSubobject<UPlusSphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionResponseToChannel(INTERACT, ECR_Block);
}

// Called when the game starts or when spawned
void APlus::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlus::Interact()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Interact"));
	}
	auto Board = Cast<ABoard>(this->GetAttachParentActor());
	/*if (Board) {
		Board->AddParts(this);
	}*/
	ButtonStrategyComponent->AddParts(Board);
}

FVector APlus::GetLocalObjectSize()
{
	if (StaticMeshComponent) {
		FVector WorldObjectSize = StaticMeshComponent->Bounds.GetBox().GetSize();
		return WorldObjectSize;
	}
	return FVector();
}

void APlus::InitPlusButtonStrategy(TSubclassOf<UPlusButtonStrategyComponent> PlusButtonStrategy)
{
	ButtonStrategyComponent = NewObject<UPlusButtonStrategyComponent>(this, PlusButtonStrategy.Get());

	if (ButtonStrategyComponent) {
		ButtonStrategyComponent->RegisterComponent();
		ButtonStrategyComponent->InitializeComponent();
	}
}

