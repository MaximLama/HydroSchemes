// HydroSchemes Simulator. All rights reserved.


#include "HSActors/Board.h"
#include "HSActors/BoardPart.h"
#include "HSActors/Plus.h"
#include "FunctionalComponents/AddBottomPBSComponent.h"
#include "FunctionalComponents/AddLeftPBSComponent.h"
#include "FunctionalComponents/AddRightPBSComponent.h"
#include "FunctionalComponents/AddTopPBSComponent.h"
#include "HSActors/BoardBoundingBoxComponent.h"
#include "HSActors/BoardSchemeActor.h"

DEFINE_LOG_CATEGORY_STATIC(BaseBoardLog, All, All)

// Sets default values
ABoard::ABoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//—оздание компонентов доски
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoardBoundingBoxComponent>("BoxComponent");

	BoxComponent->SetupAttachment(GetRootComponent());
}

void ABoard::UpdateBoxComponent()
{
	BoxComponent->UpdateBoxExtent();
}

ABoardPart* ABoard::AddPart(const FVector& RelativeLocation)
{
	auto BoardPart = GetWorld()->SpawnActor<ABoardPart>(BoardPartClass);
	BoardPart->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	BoardPart->SetActorRelativeLocation(RelativeLocation);
	BoxComponent->IncreaseBoundingBox(BoardPart);
	return BoardPart;
}

void ABoard::AddBoxOffset(const FVector& Offset)
{
	BoxComponent->AddWorldOffset(Offset);
}

void ABoard::Tick(float DeltaTime)
{}

bool ABoard::CanPlaceHolo(ABoardSchemeActor* HoloActor)
{
	auto BoardPart = Cast<ABoardPart>(HoloActor->GetAttachParentActor());
	int32 Row = -1;
	int32 Column = -1;
	FindIndexesByObject(Row, Column, BoardPart);
	if (Row == -1 || Column == -1) return false;
	if (Row + HoloActor->GetHeight() - 1 < PartsMatrix.Num() && Column + HoloActor->GetWidth() - 1 < PartsMatrix[0].Num()) {
		for (int32 i = Row; i < Row + HoloActor->GetHeight(); i++) {
			for (int32 j = Column; j < Column + HoloActor->GetWidth(); j++) {
				if (PartsMatrix[i][j]->IsEmployed) return false;
			}
		}
		return true;
	}
	return false;
}

bool ABoard::CanPlaceHolo(ABoardSchemeActor* HoloActor, int32& Row, int32& Column)
{
	auto BoardPart = Cast<ABoardPart>(HoloActor->GetAttachParentActor());
	Row = -1;
	Column = -1;
	FindIndexesByObject(Row, Column, BoardPart);
	if (Row == -1 || Column == -1) return false;
	if (Row + HoloActor->GetHeight() - 1 < PartsMatrix.Num() && Column + HoloActor->GetWidth() - 1 < PartsMatrix[0].Num()) {
		for (int32 i = Row; i < Row + HoloActor->GetHeight(); i++) {
			for (int32 j = Column; j < Column + HoloActor->GetWidth(); j++) {
				if (PartsMatrix[i][j]->IsEmployed) return false;
			}
		}
		return true;
	}
	return false;
}

void ABoard::EmployBoardParts(int32 Row, int32 Column, int32 Width, int32 Height)
{
	if (!(FMath::IsWithinInclusive(Row, 0, PartsMatrix.Num() - 1)
		&& FMath::IsWithinInclusive(Column, 0, PartsMatrix[0].Num() - 1)
		&& FMath::IsWithinInclusive(Row + Height - 1, 0, PartsMatrix.Num() - 1)
		&& FMath::IsWithinInclusive(Column + Width - 1, 0, PartsMatrix[0].Num()))) return;

	for (int32 i = Row; i < Row + Height; i++) {
		for (int32 j = Column; j < Column + Width; j++) {
			PartsMatrix[i][j]->IsEmployed = true;
		}
	}
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
	
	InitBoardPartSize();
	if (BoardPartSize.IsNearlyZero()) return;

	InitBoard();
	InitButtons();
}

void ABoard::InitBoardPartSize()
{
	if (!GetWorld()) return;
	//ƒл€ получени€ размеров создаем временную часть доски, получаем ее размеры и удал€ем ее
	auto TempBoardPart = GetWorld()->SpawnActor<ABoardPart>(BoardPartClass);
	if (!TempBoardPart) return;
	BoardPartSize = TempBoardPart->GetLocalObjectSize();
	TempBoardPart->Destroy();
}

TArray<TArray<TObjectPtr<ABoardPart>>>* ABoard::GetParts()
{
	return &PartsMatrix;
}

void ABoard::InitBoard()
{
	//FirstPartLocation - положение левой нижней части доски относительно локальной системы координат доски
	FVector FirstPartLocation = FVector((RowPartsCount / 2.0f - 0.5) * BoardPartSize.X, (-ColumnPartsCount / 2.0f + 0.5) * BoardPartSize.X, 0.0f);
	//—оздание частей доски в цикле
	if (RowPartsCount && ColumnPartsCount) {
		for (int32 i = 0; i < RowPartsCount; i++) {
			PartsMatrix.Add(TArray<TObjectPtr<ABoardPart>>());
			for (int32 j = 0; j < ColumnPartsCount; j++) {
				PartsMatrix[i].Add(
					AddPart(
						FVector(
							(RowPartsCount / 2.0f - i - 0.5) * BoardPartSize.X,
							(-ColumnPartsCount / 2.0f + j + 0.5) * BoardPartSize.X,
							0.0f
						)
					)
				);
			}
		}
	}
	//ќбновление размера Box Component
	UpdateBoxComponent();
}

void ABoard::InitButtons()
{
	//ѕолучение размера кнопки плюс
	auto TempButton = GetWorld()->SpawnActor<APlus>(PlusPartClass);
	FVector PlusButtonBoxSize = TempButton->GetLocalObjectSize();
	TempButton->Destroy();

	//ѕолучение смещение кнопки относительно локального центра доски
	FVector BoxComponentExtent = BoxComponent->GetScaledBoxExtent();
	float ScaledRelativeLocation = ScaledPlusButtonOffset * PlusButtonBoxSize.X + BoxComponentExtent.X;
	
	//—оздание 4 кнопок и присвоение им стратегии добавлени€ частей доски
	ButtonUp = SpawnButton(GetActorTransform().GetUnitAxis(EAxis::X) * ScaledRelativeLocation);
	if (ButtonUp) 
		ButtonUp->InitPlusButtonStrategy(UAddTopPBSComponent::StaticClass());
	ButtonDown = SpawnButton(-GetActorTransform().GetUnitAxis(EAxis::X) * ScaledRelativeLocation);
	if(ButtonDown)
		ButtonDown->InitPlusButtonStrategy(UAddBottomPBSComponent::StaticClass());
	ButtonRight = SpawnButton(GetActorTransform().GetUnitAxis(EAxis::Y) * ScaledRelativeLocation);
	if(ButtonRight)
		ButtonRight->InitPlusButtonStrategy(UAddRightPBSComponent::StaticClass());
	ButtonLeft = SpawnButton(-GetActorTransform().GetUnitAxis(EAxis::Y) * ScaledRelativeLocation);
	if(ButtonLeft)
		ButtonLeft->InitPlusButtonStrategy(UAddLeftPBSComponent::StaticClass());
}

APlus* ABoard::SpawnButton(const FVector& RelativeLocation)
{
	auto Button = GetWorld()->SpawnActor<APlus>(PlusPartClass);
	Button->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	Button->SetActorRelativeLocation(FVector());
	Button->AddActorWorldOffset(RelativeLocation);
	return Button;
}

void ABoard::FindIndexesByObject(int32& Row, int32& Column, ABoardPart* BoardPart)
{
	if (!BoardPart) return;
	auto FirstBoardPart = PartsMatrix[0][0];
	FVector FirstLocation = FirstBoardPart->GetActorTransform().GetRelativeTransform(GetActorTransform()).GetTranslation();
	FVector FindBoardPartLocation = BoardPart->GetActorTransform().GetRelativeTransform(GetActorTransform()).GetTranslation();
	Row = FMath::RoundToInt32((FirstLocation.X - FindBoardPartLocation.X) / GetBoardPartSize().X);
	Column = FMath::RoundToInt32((FindBoardPartLocation.Y - FirstLocation.Y) / GetBoardPartSize().X);
}

