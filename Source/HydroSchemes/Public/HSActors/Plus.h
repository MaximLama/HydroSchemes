// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "Plus.generated.h"

class UPlusButtonStrategyComponent;
class UPlusSphereComponent;

UCLASS()
class HYDROSCHEMES_API APlus : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlusButtonStrategyComponent> ButtonStrategyComponent;
	
public:	
	// Sets default values for this actor's properties
	APlus();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlusSphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	virtual void Interact() override;

	UFUNCTION()
	FVector GetLocalObjectSize();

	UFUNCTION()
	void InitPlusButtonStrategy(TSubclassOf<UPlusButtonStrategyComponent> PlusButtonStrategy);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
