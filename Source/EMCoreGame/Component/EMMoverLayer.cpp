#include "EMMoverLayer.h"

#include "MotionWarpingComponent.h"
#include "MoverComponent.h"
#include "MoverTypes.h"
#include "Curves/CurveVector.h"
#include "DefaultMovementSet/LayeredMoves/AnimRootMotionLayeredMove.h"
#include "MoveLibrary/MovementUtils.h"

// -------------------------------------------------------------------
// FEMLayeredMove_MoveTo
// -------------------------------------------------------------------
FEMLayeredMove_MoveTo::FEMLayeredMove_MoveTo()
	: StartLocation(ForceInitToZero)
	, TargetLocation(ForceInitToZero)
	, bRestrictSpeedToExpected(false)
	, PathOffsetCurve(nullptr)
	, TimeMappingCurve(nullptr)
{
	DurationMs = 1000.f;
	MixMode = EMoveMixMode::OverrideVelocity;
}

FVector FEMLayeredMove_MoveTo::GetPathOffsetInWorldSpace(const float moveFraction) const
{
	if (PathOffsetCurve)
	{
		float minCurveTime(0.f);
		float maxCurveTime(1.f);

		PathOffsetCurve->GetTimeRange(minCurveTime, maxCurveTime);
		
		const FVector pathOffsetInFacingSpace = PathOffsetCurve->GetVectorValue(FMath::GetRangeValue(FVector2f(minCurveTime, maxCurveTime), moveFraction));;
		FRotator facingRotation((TargetLocation-StartLocation).Rotation());
		facingRotation.Pitch = 0.f;
		return facingRotation.RotateVector(pathOffsetInFacingSpace);
	}

	return FVector::ZeroVector;
}

float FEMLayeredMove_MoveTo::EvaluateFloatCurveAtFraction(const UCurveFloat& curve, const float fraction) const
{
	float minCurveTime(0.f);
	float maxCurveTime(1.f);

	curve.GetTimeRange(minCurveTime, maxCurveTime);
	return curve.GetFloatValue(FMath::GetRangeValue(FVector2f(minCurveTime, maxCurveTime), fraction));
}

bool FEMLayeredMove_MoveTo::GenerateMove(const FMoverTickStartData& startState, const FMoverTimeStep& timeStep, const UMoverComponent* moverComp, UMoverBlackboard* simBlackboard, FProposedMove& outProposedMove)
{
	const FMoverDefaultSyncState* syncState = startState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	if (!syncState)
	{
		return false;
	}

	const FCharacterDefaultInputs* characterInputs = startState.InputCmd.InputCollection.FindDataByType<FCharacterDefaultInputs>();
	if (!characterInputs)
	{
		return false;
	}

	outProposedMove = FProposedMove();
	outProposedMove.MixMode = MixMode;
	
	const float deltaSeconds = timeStep.StepMs / 1000.f;

	FVector velocity = (TargetLocation - StartLocation) / (DurationMs / 1000.f);

	outProposedMove.LinearVelocity = velocity;
	
	FRotator IntendedOrientation_WorldSpace;
	if (!characterInputs || characterInputs->OrientationIntent.IsNearlyZero())
	{
		IntendedOrientation_WorldSpace = syncState->GetOrientation_WorldSpace();
	}
	else
	{
		IntendedOrientation_WorldSpace = characterInputs->GetOrientationIntentDir_WorldSpace().ToOrientationRotator();
	}
	
	outProposedMove.AngularVelocityDegrees = UMovementUtils::ComputeAngularVelocityDegrees(syncState->GetOrientation_WorldSpace(), IntendedOrientation_WorldSpace, deltaSeconds, -1.f);
	return true;
}

FLayeredMoveBase* FEMLayeredMove_MoveTo::Clone() const
{
	FEMLayeredMove_MoveTo* copyPtr = new FEMLayeredMove_MoveTo(*this);
	return copyPtr;
}

void FEMLayeredMove_MoveTo::NetSerialize(FArchive& ar)
{
	Super::NetSerialize(ar);

	ar << StartLocation;
	ar << TargetLocation;
	ar << bRestrictSpeedToExpected;
	ar << PathOffsetCurve;
	ar << TimeMappingCurve;
}

UScriptStruct* FEMLayeredMove_MoveTo::GetScriptStruct() const
{
	return FEMLayeredMove_MoveTo::StaticStruct();
}

FString FEMLayeredMove_MoveTo::ToSimpleString() const
{
	return FString::Printf(TEXT("EMMove To"));
}

void FEMLayeredMove_MoveTo::AddReferencedObjects(FReferenceCollector& collector)
{
	Super::AddReferencedObjects(collector);
}

// ----------------------------------------------------------------------------
// FEMLayeredMove_AnimRootMotion
// ----------------------------------------------------------------------------
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mover_EMAnimRootMotion_Montage, "Mover.EMAnimRootMotion.Montage", "Signifies an association with root motion that comes via a montage")
FEMLayeredMove_AnimRootMotion::FEMLayeredMove_AnimRootMotion()
{
	DurationMs = 0.f;
	MixMode = EMoveMixMode::OverrideAll;
}

bool FEMLayeredMove_AnimRootMotion::GenerateMove(const FMoverTickStartData& simState, const FMoverTimeStep& timeStep, const UMoverComponent* moverComp, UMoverBlackboard* simBlackboard, FProposedMove& outProposedMove)
{
	float meshMontagePos = -1.f;
	if (!timeStep.bIsResimulating)
	{
		bool bIsMontageStillPlaying = false;

		if (const USkeletalMeshComponent* meshComp = Cast<USkeletalMeshComponent>(moverComp->GetPrimaryVisualComponent()))
		{
			if (const UAnimInstance* meshAnimInstance = meshComp->GetAnimInstance())
			{
				bIsMontageStillPlaying = MontageState.Montage && meshAnimInstance->Montage_IsPlaying(MontageState.Montage);
				if (bIsMontageStillPlaying)
				{
					meshMontagePos = meshAnimInstance->Montage_GetPosition(MontageState.Montage);
				}
			}
		}

		if (!bIsMontageStillPlaying)
		{
			DurationMs = 0.f;
			return false;
		}
	}

	const float deltaSeconds = timeStep.StepMs / 1000.f;

	const FMoverDefaultSyncState* syncState = simState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	if (!syncState)
	{
		return false;
	}

	const FCharacterDefaultInputs* characterInputs = simState.InputCmd.InputCollection.FindDataByType<FCharacterDefaultInputs>();
	if (!characterInputs)
	{
		return false;
	}

	const float montageRateScale = (MontageState.Montage ? MontageState.Montage->RateScale : 1.f);
	const double secondsSinceMontageStarted = (timeStep.BaseSimTimeMs - StartSimTimeMs) / 1000.f;
	const double scaledSecondsSinceMontageStarted = secondsSinceMontageStarted * MontageState.PlayRate * montageRateScale;

	const float extractionStartPosition = MontageState.StartingMontagePosition + scaledSecondsSinceMontageStarted;
	const float extractionEndPosition   = extractionStartPosition + (deltaSeconds * MontageState.PlayRate * montageRateScale);

	const FTransform localRootMotion = IsValid(MontageState.Montage) ? UMotionWarpingUtilities::ExtractRootMotionFromAnimation(MontageState.Montage, extractionStartPosition, extractionEndPosition) : FTransform::Identity;

	FMotionWarpingUpdateContext warpingContext;
	warpingContext.Animation = MontageState.Montage;
	warpingContext.CurrentPosition = extractionEndPosition;
	warpingContext.PreviousPosition = extractionStartPosition;
	warpingContext.PlayRate = MontageState.PlayRate * montageRateScale;
	warpingContext.Weight = 1.f;

	const FTransform simActorTransform = FTransform(syncState->GetOrientation_WorldSpace().Quaternion(), syncState->GetLocation_WorldSpace());
	const FTransform worldSpaceRootMotion = moverComp->ConvertLocalRootMotionToWorld(localRootMotion, deltaSeconds, &simActorTransform, &warpingContext);

	outProposedMove = FProposedMove();
	outProposedMove.MixMode = MixMode;

	if (deltaSeconds > UE_KINDA_SMALL_NUMBER)
	{
		outProposedMove.LinearVelocity = worldSpaceRootMotion.GetTranslation() / deltaSeconds;
		const FQuat shortestArcRootRotation = worldSpaceRootMotion.GetRotation().GetShortestArcWith(FQuat::Identity);
		outProposedMove.AngularVelocityDegrees   = FMath::RadiansToDegrees(shortestArcRootRotation.ToRotationVector() / deltaSeconds);

		FRotator IntendedOrientation_WorldSpace;
		if (!characterInputs || characterInputs->OrientationIntent.IsNearlyZero())
		{
			IntendedOrientation_WorldSpace = syncState->GetOrientation_WorldSpace();
		}
		else
		{
			IntendedOrientation_WorldSpace = characterInputs->GetOrientationIntentDir_WorldSpace().ToOrientationRotator();
		}
	
		outProposedMove.AngularVelocityDegrees = UMovementUtils::ComputeAngularVelocityDegrees(syncState->GetOrientation_WorldSpace(), IntendedOrientation_WorldSpace, deltaSeconds, -1.f);

	}

	MontageState.CurrentPosition = extractionEndPosition;
	return true;
}

bool FEMLayeredMove_AnimRootMotion::HasGameplayTag(FGameplayTag tagToFind, bool bExactMatch) const
{
	static const FGameplayTag MontageAttributeTag = Mover_EMAnimRootMotion_Montage.GetTag();
	const bool bFoundMatch = bExactMatch ? MontageAttributeTag.MatchesTagExact(tagToFind) : MontageAttributeTag.MatchesTag(tagToFind);

	return bFoundMatch || Super::HasGameplayTag(tagToFind, bExactMatch);
}

FLayeredMoveBase* FEMLayeredMove_AnimRootMotion::Clone() const
{
	FEMLayeredMove_AnimRootMotion* copyPtr = new FEMLayeredMove_AnimRootMotion(*this);
	return copyPtr;
}

UScriptStruct* FEMLayeredMove_AnimRootMotion::GetScriptStruct() const
{
	return FEMLayeredMove_AnimRootMotion::StaticStruct();
}

FString FEMLayeredMove_AnimRootMotion::ToSimpleString() const
{
	return FString::Printf(TEXT("EMAnimRootMotion"));
}