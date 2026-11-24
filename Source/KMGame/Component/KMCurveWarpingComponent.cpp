#include "KMCurveWarpingComponent.h"
#include "Component/EMMoverCurveWarpingFallingMode.h"
#include "Component/EMMoverLayer.h"
#include "Component/EMPawnMoverComponent.h"
#include "DefaultMovementSet/LayeredMoves/BasicLayeredMoves.h"
#include "DefaultMovementSet/LayeredMoves/LaunchMove.h"
#include "GameActor/Pawn/Character/KMPawnBase.h"

UKMCurveWarpingComponent::UKMCurveWarpingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCurveWarpingComponent::SetOwnerMovementMode(EMovementMode newMovementMode)
{
	if (AKMPawnBase* ownerPawn = Cast<AKMPawnBase>(GetOwner()))
	{
		ownerPawn->SetMovementMode(MOVE_Custom);
	}
	else if (UEMPawnMoverComponent* moverComponent = GetOwner()->FindComponentByClass<UEMPawnMoverComponent>())
	{
		if (newMovementMode == MOVE_Custom)
		{
			moverComponent->QueueNextMode(UEMMoverCurveWarpingFallingMode::ModeName);
		}
	}
}

int32 UKMCurveWarpingComponent::PlayLinearWarp(FVector targetWarpLocation, float newDuration)
{
	if (UEMPawnMoverComponent* moverComponent = GetOwner()->FindComponentByClass<UEMPawnMoverComponent>())
	{
		//moverComponent->CancelFeaturesWithTag(Mover_AnimRootMotion_Montage, true);
		
		TSharedPtr<FEMLayeredMove_MoveTo> newMoveTo = MakeShared<FEMLayeredMove_MoveTo>();

		newMoveTo->MixMode = EMoveMixMode::OverrideAllExceptVerticalVelocity;
		newMoveTo->StartLocation = GetOwner()->GetActorLocation();
		newMoveTo->TargetLocation = targetWarpLocation;
		newMoveTo->DurationMs = newDuration * 1000.f;
		newMoveTo->bRestrictSpeedToExpected = true;
		newMoveTo->Priority = 2;

		moverComponent->QueueLayeredMove(newMoveTo);
			
		return INDEX_NONE;
	}
	return Super::PlayLinearWarp(targetWarpLocation, newDuration);
}