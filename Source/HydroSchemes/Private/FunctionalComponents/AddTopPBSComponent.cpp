// HydroSchemes Simulator. All rights reserved.


#include "FunctionalComponents/AddTopPBSComponent.h"
#include "HSActors/Board.h"
#include "HSActors/BoardPart.h"
#include "HSActors/Plus.h"

void UAddTopPBSComponent::AddParts(ABoard* Board)
{
	auto Parts = Board->GetParts();
	TArray<TObjectPtr<ABoardPart>> NewPartRow;
	int32 ColumnsCount = (*Parts)[0].Num();
	FVector InitialLocation = (*Parts)[0][0]->GetActorTransform().GetRelativeTransform(Board->GetActorTransform()).GetTranslation();
	for (int32 i = 0; i < ColumnsCount; i++) {
		auto BoardPart = Board->AddPart(FVector(InitialLocation.X + Board->GetBoardPartSize().X, InitialLocation.Y + i * Board->GetBoardPartSize().X, 0));
		NewPartRow.Add(BoardPart);
	}
	Parts->Insert(NewPartRow, 0);
	GetOwner()->AddActorWorldOffset(Board->GetActorForwardVector() * Board->GetBoardPartSize().X);
	Board->ButtonLeft->AddActorWorldOffset(Board->GetActorForwardVector() * Board->GetBoardPartSize().X / 2);
	Board->ButtonRight->AddActorWorldOffset(Board->GetActorForwardVector() * Board->GetBoardPartSize().X / 2);
	Board->AddBoxOffset(Board->GetActorForwardVector() * (Board->GetBoardPartSize().X / 2));
	Board->UpdateBoxComponent();
}