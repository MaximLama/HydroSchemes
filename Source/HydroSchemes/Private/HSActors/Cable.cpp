// HydroSchemes Simulator. All rights reserved.


#include "HSActors/Cable.h"
#include "CableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "HSActors/BoardSchemeActor.h"
#include "Player/HUBaseCharacter.h"
#include "Engine/Engine.h"

#define INTERACT ECC_GameTraceChannel1

ACable::ACable()
{
	PrimaryActorTick.bCanEverTick = true;

	CableInputLeft = CreateDefaultSubobject<UStaticMeshComponent>("CableInputLeft");
	CableInputLeft->SetupAttachment(RootComponent);
	CableInputLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	CableInputRight = CreateDefaultSubobject<UStaticMeshComponent>("CableInputRight");
	CableInputRight->SetupAttachment(RootComponent);
	CableInputRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	Cable = CreateDefaultSubobject<UCableComponent>("Cable");
	Cable->SetupAttachment(CableInputLeft);
	Cable->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

void ACable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACable::AttachToCharacter(AHUBaseCharacter* Character, const FAttachmentTransformRules& AttachmentRules, FName SocketName)
{
	InitializeForm();
	Super::AttachToCharacter(Character, AttachmentRules, SocketName);
}

void ACable::ShowHolo(AHUBaseCharacter* Character, const FHitResult HitResult)
{
	auto SchemeActor = Cast<ABoardSchemeActor>(HitResult.GetActor());
	if (!SchemeActor) return;
	if (!Character->Holo || Character->Holo->IsActorBeingDestroyed()) {
		FName NearestSocketOutput = GetNearestSocketName(SchemeActor, HitResult.ImpactPoint);
		if (NearestSocketOutput == NAME_None) return;

		Character->Holo = CreateHolo(this, Character->HoloMaterial);

		if (Character->Holo) {
			auto Holo = Cast<ACable>(Character->Holo);
			Holo->AttachToBoardActor(SchemeActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NearestSocketOutput);
			Character->SetHoloState(EHoloState::Correct);
			Character->CheckHoloState(SchemeActor, NearestSocketOutput);
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, NearestSocketOutput.ToString());
			}
		}
	}
	if (Character->Holo && !Character->Holo->IsActorBeingDestroyed()) {
		auto Holo = Cast<ACable>(Character->Holo);
		FName NearestSocketOutput = GetNearestSocketName(SchemeActor, HitResult.ImpactPoint);
		if (Character->Holo->GetAttachParentActor() != SchemeActor || 
			(SchemeActor->IsSocketOutputExist(NearestSocketOutput) &&
				!SchemeActor->IsSocketOutputEmployed(NearestSocketOutput))) {
			
			if (Holo->bIsCableLeftFixed) {
				FVector RelativeLocation = Holo->CableInputRight->GetAttachParent()->GetSocketTransform(Holo->CableInputRight->GetAttachSocketName())
					.InverseTransformPosition(SchemeActor->GetSocketLocationByName(NearestSocketOutput));
				RelativeLocation.Z += Holo->CableInputLeft->GetRelativeLocation().Z;
				Holo->CableInputRight->SetRelativeLocation(RelativeLocation);
				float Distance = FVector::Distance(Holo->CableInputLeft->GetRelativeLocation(), Holo->CableInputRight->GetRelativeLocation());
				Holo->Cable->CableLength = FMath::Clamp(Distance, 0.f, 1000.f);
				Character->CheckHoloState(SchemeActor, NearestSocketOutput);
				Holo->CableRightAttachActor = SchemeActor;
				Holo->CableRightAttachSocketName = NearestSocketOutput;
				if (GEngine) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, NearestSocketOutput.ToString());
				}
			}
			else {
				Character->Holo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Holo->AttachToBoardActor(SchemeActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NearestSocketOutput);
				Character->CheckHoloState(SchemeActor, NearestSocketOutput);
				if (GEngine) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, NearestSocketOutput.ToString());
				}
			}
		}
	}
}

ASchemeActor* ACable::CreateHolo(ASchemeActor* PlacingElement, UMaterialInterface* HoloMaterial)
{
	if (!GetWorld()) return nullptr;
	auto Holo = GetWorld()->SpawnActor<ACable>(PlacingElement->GetClass());
	if (!Holo) return nullptr;
	Holo->InitializeForm();
	Holo->SetResponseToChannel(INTERACT, ECR_Ignore);
	Holo->SetRCT();
	TWeakObjectPtr<UMaterialInstanceDynamic> HoloDynMatInst = UMaterialInstanceDynamic::Create(HoloMaterial, Holo);
	Holo->SetHoloMaterialInstance(HoloDynMatInst);
	return Holo;
}

void ACable::InitializeForm()
{
	CableInputLeft->SetRelativeTransform(CableLeftInitialTransform);
	CableInputRight->SetRelativeTransform(CableRightInitialTransform);
	Cable->CableLength = FVector::Distance(CableLeftInitialTransform.GetLocation(), CableRightInitialTransform.GetLocation());
}

void ACable::AttachToBoardActor(ABoardSchemeActor* SchemeActor, const FAttachmentTransformRules& AttachmentRules, const FName SocketName)
{
	UStaticMeshComponent* ParentSocketComponent = SchemeActor->GetCableSocketComponent(SocketName);
	AttachToComponent(ParentSocketComponent, AttachmentRules, SocketName);
}

void ACable::SetResponseToChannel(ECollisionChannel CollisionChannel, ECollisionResponse CollisionResponse)
{
	CableInputLeft->SetCollisionResponseToChannel(CollisionChannel, CollisionResponse);
	CableInputRight->SetCollisionResponseToChannel(CollisionChannel, CollisionResponse);
	Cable->SetCollisionResponseToChannel(CollisionChannel, CollisionResponse);
}

void ACable::SetRCT()
{
	CableInputLeft->SetRenderCustomDepth(true);
	CableInputRight->SetRenderCustomDepth(true);
	Cable->SetRenderCustomDepth(true);
}

void ACable::SetHoloMaterialInstance(TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstance)
{
	HoloMaterialInstance = MaterialInstance;
	for (int32 i = 0; i < CableInputLeft->GetNumMaterials(); i++) {
		CableInputLeft->SetMaterial(i, HoloMaterialInstance.Get());
	}
	for (int32 i = 0; i < CableInputRight->GetNumMaterials(); i++) {
		CableInputRight->SetMaterial(i, HoloMaterialInstance.Get());
	}
	Cable->SetMaterial(0, HoloMaterialInstance.Get());
}

FName ACable::GetNearestSocketName(ABoardSchemeActor* SchemeActor, FVector ImpactPoint)
{
	FName NearestSocketOutput;
	float NearestSocketDistance = MAX_FLT;

	TArray<FName> SocketOutputs = SchemeActor->GetAllSocketsForCable();
	for (const FName& SocketOutput : SocketOutputs) {
		FVector SocketLocation = SchemeActor->GetSocketLocationByName(SocketOutput);
		float Distance = FVector::Distance(ImpactPoint, SocketLocation);

		if (Distance < NearestSocketDistance) {
			NearestSocketDistance = Distance;
			NearestSocketOutput = SocketOutput;
		}
	}
	return NearestSocketOutput;
}

void ACable::PlaceElement(AHUBaseCharacter* Character)
{
	auto Holo = Cast<ACable>(Character->Holo);
	if (Holo) {
		if (!Holo->bIsCableLeftFixed) Holo->bIsCableLeftFixed = true;
		else {
			auto LeftParentActor = Cast<ABoardSchemeActor>(Holo->GetAttachParentActor());
			auto RightParentActor = Holo->CableRightAttachActor;
			if (!(LeftParentActor->IsSocketOutputEmployed(Holo->GetAttachParentSocketName()) || RightParentActor->IsSocketOutputEmployed(Holo->CableRightAttachSocketName))) {
				auto CableActor = GetWorld()->SpawnActor<ACable>(Character->PlacingElement->GetClass());
				if (CableActor) {
					CableActor->InitializeForm();
					CableActor->AttachToBoardActor(LeftParentActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Holo->GetAttachParentSocketName());
					CableActor->CableInputRight->SetRelativeLocation(Holo->CableInputRight->GetRelativeLocation());
					CableActor->CableRightAttachActor = RightParentActor;
					CableActor->CableRightAttachSocketName = Holo->CableRightAttachSocketName;
					CableActor->Cable->CableLength = Holo->Cable->CableLength;
					CableActor->bIsPickUpAble = false;
					LeftParentActor->EmploySocketOutputByName(CableActor->GetAttachParentSocketName());
					LeftParentActor->SetSocketRelatedActor(CableActor->GetAttachParentSocketName().ToString(), RightParentActor, CableActor->CableRightAttachSocketName.ToString());
					RightParentActor->EmploySocketOutputByName(CableActor->CableRightAttachSocketName);
					RightParentActor->SetSocketRelatedActor(CableActor->CableRightAttachSocketName.ToString(), LeftParentActor, CableActor->GetAttachParentSocketName().ToString());
					Character->DestroyHolo();
				}
			}
		}
	}
}
