// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSActors/SchemeActor.h"
#include "Misc/HashBuilder.h"
#include "Utils/SchemeUtil.h"
#include "BoardSchemeActor.generated.h"

DECLARE_MULTICAST_DELEGATE(FSocketDelegate)

class ACable;
class ABoardSchemeActor;

USTRUCT(BlueprintType)
struct FRelatedActorData
{
	GENERATED_BODY()

public:
	ABoardSchemeActor* RelatedActor;
	FString RelatedActorSocket;

	FRelatedActorData(ABoardSchemeActor* RelActor = nullptr, FString RelActorSocket = FString()): RelatedActor(RelActor), RelatedActorSocket(RelActorSocket){}

	bool operator==(const FRelatedActorData& Other) const {
		return RelatedActor == Other.RelatedActor && RelatedActorSocket.Equals(Other.RelatedActorSocket);
	}

	FRelatedActorData& operator=(const FRelatedActorData& Other) {
		if (this != &Other) {
			RelatedActor = Other.RelatedActor;
			RelatedActorSocket = Other.RelatedActorSocket;
		}
		return *this;
	}

	bool IsValid() {
		return RelatedActor != nullptr && !RelatedActorSocket.IsEmpty();
	}

	friend uint32 GetTypeHash(const FRelatedActorData& Other) {
		uint32 Hash = 0;
		HashCombine(Hash, GetTypeHash(Other.RelatedActor));
		HashCombine(Hash, GetTypeHash(Other.RelatedActorSocket));
		return Hash;
	}

	FString ToString();
};

USTRUCT(BlueprintType)
struct FBoardActorOutput
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FName OutputName;

	UPROPERTY(VisibleAnywhere)
	bool bIsEmployed = false;

	UPROPERTY(VisibleAnywhere)
	float Pressure = 0.f;

	FRelatedActorData RelatedActorData;

	UPROPERTY(VisibleAnywhere)
	bool bOutMode = false;

	FSocketDelegate Delegate;

	FString ToString() {
		FString Result;
		Result += "Name: " + OutputName.ToString() + "\n";
		Result += "bIsEmployed: ";
		Result += bIsEmployed ? "true" : "false";
		Result += "\n";
		Result += "Pressure: " + FString::SanitizeFloat(Pressure) + "\n";
		Result += "RelatedActorData: {\n" + RelatedActorData.ToString() + "}\n";
		Result += "bOutMode: ";
		Result += bOutMode ? "true" : "false";
		Result += "\n";
		return Result;
	}
};

USTRUCT()
struct FSocketRelations
{
	GENERATED_BODY()

public:
	TArray<FString> SocketRelations;

	FString ToString() {
		FString Result;
		for (FString SocketRelation : SocketRelations) {
			Result += SocketRelation + " ";
		}
		return Result;
	}

	bool IsEmpty() const{
		return SocketRelations.Num() == 0;
	}
};

USTRUCT()
struct FSocketRelationsScheme
{
	GENERATED_BODY()

public:
	TMap<FString, FSocketRelations> SocketRelationsScheme;

	FString ToString() {
		FString Result = "SocketRelationScheme - Output\n----------\n";
		for (TPair<FString, FSocketRelations> SocketRelation : SocketRelationsScheme) {
			Result += SocketRelation.Key + " - " + SocketRelation.Value.ToString() + "\n";
		}
		return Result;
	}
};

/**
 * 
 */
UCLASS(Abstract)
class HYDROSCHEMES_API ABoardSchemeActor : public ASchemeActor
{
	GENERATED_BODY()

private:
	int32 WidthInCell = 0;
	int32 HeightInCell = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere)
	TMap<FString, FBoardActorOutput> SocketOutputs;
	FBoardActorOutput* GetSocketInfo(FString SocketName);

	TArray<FSocketRelationsScheme> SocketRelationsSchemes;
	UPROPERTY(VisibleAnywhere)
	FSocketRelationsScheme CurrentScheme;
	bool IsCurrentSchemeEmpty() const;
	const FSocketRelations* GetCurrentSocketRelations(FString SocketName) const;

	ABoardSchemeActor();
	void InitSize(float CellSize);
	inline int32 GetWidth() const { return WidthInCell; }
	inline int32 GetHeight() const { return HeightInCell; }
	virtual void SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse) override;
	virtual void SetRCT() override;
	virtual void ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult) override;
	virtual ASchemeActor* CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial) override;
	virtual TArray<FName> GetAllSocketsForCable();
	virtual FVector GetSocketLocationByName(FName SocketName);
	virtual UStaticMeshComponent* GetCableSocketComponent(const FName& SocketName) const;
	bool CanPlaceHolo(FName SocketName);
	bool IsSocketOutputExist(FName SocketName);
	bool IsSocketOutputEmployed(FName SocketName);
	virtual void PlaceElement(AHUBaseCharacter* Character) override;
	void EmploySocketOutputByName(FName SocketName);
	void ReleaseSocketOutputByName(FName SocketName);
	virtual void ChangeState() {};
	void SetOutputPressure(FString SocketName, float Pressure);
	void SetInputPressure(FString SocketName, float Pressure);
	virtual void OnSetOutputPressureAfter(FString SocketName, float Pressure);
	virtual void OnSetInputPressureAfter(FString SocketName, float Pressure);
	void SetSocketRelatedActor(FString ThisSocketName, ABoardSchemeActor* RelatedActor, FString RelatedSocketName);
	void CheckPressure();
	void SocketBroadcast();
	FString PrintSocketOutputs();
};