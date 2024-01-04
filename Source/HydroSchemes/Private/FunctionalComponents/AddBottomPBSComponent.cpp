// HydroSchemes Simulator. All rights reserved.


#include "FunctionalComponents/AddBottomPBSComponent.h"
#include "HSActors/Board.h"
#include "HSActors/BoardPart.h"
#include "HSActors/Plus.h"

void UAddBottomPBSComponent::AddParts(ABoard* Board)
{
	auto Parts = Board->GetParts();
	TArray<TObjectPtr<ABoardPart>> NewPartRow;
	int32 ColumnsCount = (*Parts)[0].Num();
	int32 RowsCount = Parts->Num();
	FVector InitialLocation = (*Parts)[RowsCount - 1][0]->GetActorTransform().GetRelativeTransform(Board->GetActorTransform()).GetTranslation();
	for (int32 i = 0; i < ColumnsCount; i++) {
		auto BoardPart = Board->AddPart(FVector(InitialLocation.X - Board->GetBoardPartSize().X, InitialLocation.Y + i * Board->GetBoardPartSize().X, 0));
		NewPartRow.Add(BoardPart);
	}
	Parts->Add(NewPartRow);
	GetOwner()->AddActorWorldOffset(-Board->GetActorForwardVector() * Board->GetBoardPartSize().X);
	Board->ButtonLeft->AddActorWorldOffset(-Board->GetActorForwardVector() * Board->GetBoardPartSize().X / 2);
	Board->ButtonRight->AddActorWorldOffset(-Board->GetActorForwardVector() * Board->GetBoardPartSize().X / 2);
	Board->AddBoxOffset(-Board->GetActorForwardVector() * (Board->GetBoardPartSize().X / 2));
	Board->UpdateBoxComponent();
}
