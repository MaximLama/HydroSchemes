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

	//��������� �����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	//��������� ����������� �����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	//�������� ������, � �������� ������������� ������
	UPROPERTY(EditAnywhere)
	FName SlotSocketName = "HSSlot";

	UPROPERTY(VisibleAnywhere)
	bool IsEmployed = false;

	//�������� ������� ����� �����
	UFUNCTION()
	FVector GetLocalObjectSize();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
