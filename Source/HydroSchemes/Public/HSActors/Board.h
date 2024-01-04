// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Board.generated.h"

//class ABoardPart;
class APlus;
class UBoardBoundingBoxComponent;
class ABoardPart;

UCLASS()
class HYDROSCHEMES_API ABoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoard();
	
	//Компонент сцены
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	//Компонент области взаимодействия
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoardBoundingBoxComponent> BoxComponent;

	//Класс кнопки плюс
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlus> PlusPartClass;

	//Класс части доски
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABoardPart> BoardPartClass;

	//Кнопки плюс
	UPROPERTY()
	TObjectPtr<APlus> ButtonUp;

	UPROPERTY()
	TObjectPtr<APlus> ButtonDown;

	UPROPERTY()
	TObjectPtr<APlus> ButtonLeft;

	UPROPERTY()
	TObjectPtr<APlus> ButtonRight;

	//Количество частей доски по ширине и высоте
	UPROPERTY(EditAnywhere)
	int32 RowPartsCount = 3;

	UPROPERTY(EditAnywhere)
	int32 ColumnPartsCount = 3;
	
	UPROPERTY(EditAnywhere)
	float ScaleCollision = 0.172f;

	UPROPERTY(EditAnywhere)
	float CollisionWidth = 3.0f;

	//Обновить размеры Box Component
	void UpdateBoxComponent();

	//Добавить часть доски
	ABoardPart* AddPart(const FVector& RelativeLocation);
	
	TArray<TArray<TObjectPtr<ABoardPart>>>* GetParts();

	inline FVector GetBoardPartSize() const { return BoardPartSize; }

	void AddBoxOffset(const FVector& Offset);

	virtual void Tick(float DeltaTime) override;

	bool CanPlaceHolo(class ABoardSchemeActor* HoloActor);
	bool CanPlaceHolo(class ABoardSchemeActor* HoloActor, int32& Row, int32& Column);

	void EmployBoardParts(int32 Row, int32 Column, int32 Width, int32 Height);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Получить размеры части доски
	UFUNCTION()
	void InitBoardPartSize();

private:
	//Список частей доски
	TArray<TArray<TObjectPtr<ABoardPart>>> PartsMatrix;

	//Множитель смещения кнопки плюс от края Box Component относительно размера кнопки Plus 
	UPROPERTY()
	float ScaledPlusButtonOffset = 1.5f;

	//Размер части доски
	UPROPERTY()
	FVector BoardPartSize;

	//Инициализация доски
	void InitBoard();
	//Инициализация кнопок плюс
	void InitButtons();
	//Спавн кнопки плюс
	APlus* SpawnButton(const FVector& RelativeLocation);

	void FindIndexesByObject(int32& Row, int32& Column, ABoardPart* BoardPart);
};
