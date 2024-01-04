// HydroSchemes Simulator. All rights reserved.


#include "FunctionalComponents/AddRightPBSComponent.h"
#include "HSActors/Board.h"
#include "HSActors/BoardPart.h"
#include "HSActors/Plus.h"

void UAddRightPBSComponent::AddParts(ABoard* Board)
{
	auto Parts = Board->GetParts();
	int32 RowsCount = Parts->Num();
	int32 ColumnsCount = (*Parts)[0].Num();
	FVector InitialLocation = (*Parts)[0][ColumnsCount - 1]->GetActorTransform().GetRelativeTransform(Board->GetActorTransform()).GetTranslation();
	for (int32 i = 0; i < RowsCount; i++) {
		auto BoardPart = Board->AddPart(FVector(InitialLocation.X - i * Board->GetBoardPartSize().X, InitialLocation.Y + Board->GetBoardPartSize().X, 0));
		(*Parts)[i].Add(BoardPart);
	}
	GetOwner()->AddActorWorldOffset(Board->GetActorRightVector() * Board->GetBoardPartSize().X);
	Board->ButtonUp->AddActorWorldOffset(Board->GetActorRightVector() * Board->GetBoardPartSize().X / 2);
	Board->ButtonDown->AddActorWorldOffset(Board->GetActorRightVector() * Board->GetBoardPartSize().X / 2);
	Board->AddBoxOffset(Board->GetActorRightVector() * (Board->GetBoardPartSize().X / 2));
	Board->UpdateBoxComponent();
}
