#include "KMCurveWarpingComponent.h"
#include "GameActor/Pawn/KMPawnInterface.h"

UKMCurveWarpingComponent::UKMCurveWarpingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCurveWarpingComponent::SetOwnerMovementMode(EMovementMode newMovementMode)
{
	if (IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner()))
	{
		ownerPawnInterface->SetMovementMode(MOVE_Custom);
	}
}