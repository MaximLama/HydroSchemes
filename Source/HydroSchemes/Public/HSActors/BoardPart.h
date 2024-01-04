// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardPart.generated.h"

UCLASS()
class HYDROSCHEMES_API ABoardPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardPart();

	//Компонент сцены
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	//Компонент статической сетки
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	//Название сокета, к которому прикрепляются детали
	UPROPERTY(EditAnywhere)
	FName SlotSocketName = "HSSlot";

	UPROPERTY(VisibleAnywhere)
	bool IsEmployed = false;

	//Получить размеры части доски
	UFUNCTION()
	FVector GetLocalObjectSize();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
