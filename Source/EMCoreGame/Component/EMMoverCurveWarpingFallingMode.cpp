#include "EMMoverCurveWarpingFallingMode.h"
#include "EMCurveWarpingComponent.h"
#include "MoverComponent.h"
#include "DefaultMovementSet/Settings/CommonLegacyMovementSettings.h"
#include "MoveLibrary/AirMovementUtils.h"
#include "MoveLibrary/FloorQueryUtils.h"
#include "MoveLibrary/GroundMovementUtils.h"
#include "MoveLibrary/MovementUtils.h"

UEMMoverCurveWarpingFallingMode::UEMMoverCurveWarpingFallingMode() : Super()
{
	SharedSettingsClasses.Add(UCommonLegacyMovementSettings::StaticClass());
	
	GameplayTags.AddTag(Mover_IsInAir);
	GameplayTags.AddTag(Mover_IsFlying);
}

void UEMMoverCurveWarpingFallingMode::OnRegistered(const FName newModeName, const FMoverSimContext& simContext)
{
	Super::OnRegistered(newModeName, simContext);

	CommonLegacySettings = GetMoverComponent()->FindSharedSettings<UCommonLegacyMovementSettings>();
}

void UEMMoverCurveWarpingFallingMode::OnUnregistered(const FMoverSimContext& simContext)
{
	CommonLegacySettings = nullptr;
	
	Super::OnUnregistered(simContext);
}

void UEMMoverCurveWarpingFallingMode::BindCurveWarpingComponent(UEMCurveWarpingComponent* newCurveWarpingComponent)
{
	if (IsValid(newCurveWarpingComponent))
	{
		GetMoveDeltaFallingDelegate.BindUObject(newCurveWarpingComponent, &UEMCurveWarpingComponent::GetMoveDeltaFalling);
	}
}

void UEMMoverCurveWarpingFallingMode::UnbindCurveWarpingComponent(UEMCurveWarpingComponent* curveWarpingComponent)
{
	if (IsValid(curveWarpingComponent))
	{
		GetMoveDeltaFallingDelegate.Unbind();
	}
}

void UEMMoverCurveWarpingFallingMode::GenerateMove_Implementation(const FMoverSimContext& simContext, const FMoverTickStartData& startState, const FMoverTimeStep& timeStep, FProposedMove& outProposedMove) const
{
	const UMoverComponent* moverComp = GetMoverComponent();
	const FCharacterDefaultInputs* characterInputs = startState.InputCmd.InputCollection.FindDataByType<FCharacterDefaultInputs>();
	const FMoverDefaultSyncState* startingSyncState = startState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(startingSyncState);

	const float deltaSeconds = timeStep.StepMs * 0.001f;

	FFreeMoveParams params;
	params.MoveInput = FVector::ZeroVector;
	
	FRotator intendedOrientation_WorldSpace;
	if (!characterInputs || characterInputs->OrientationIntent.IsNearlyZero())
	{
		intendedOrientation_WorldSpace = startingSyncState->GetOrientation_WorldSpace();
	}
	else
	{
		intendedOrientation_WorldSpace = characterInputs->GetOrientationIntentDir_WorldSpace().ToOrientationRotator();
	}

	intendedOrientation_WorldSpace = UMovementUtils::ApplyGravityToOrientationIntent(intendedOrientation_WorldSpace, moverComp->GetWorldToGravityTransform(), CommonLegacySettings->bShouldRemainVertical);
	
	params.OrientationIntent = intendedOrientation_WorldSpace;
	params.PriorVelocity = FVector::ZeroVector;
	params.PriorOrientation = startingSyncState->GetOrientation_WorldSpace();
	params.DeltaSeconds = deltaSeconds;
	params.TurningRate = CommonLegacySettings->TurningRate;
	params.TurningBoost = CommonLegacySettings->TurningBoost;
	params.MaxSpeed = CommonLegacySettings->MaxSpeed;
	params.Acceleration = CommonLegacySettings->Acceleration;
	params.Deceleration = FallingDeceleration;
	params.WorldToGravityQuat = moverComp->GetWorldToGravityTransform();
	params.bUseAccelerationForVelocityMove = CommonLegacySettings->bUseAccelerationForVelocityMove;
	params.Friction = FallingLateralFriction;

	outProposedMove = UAirMovementUtils::ComputeControlledFreeMove(params);
	outProposedMove.LinearVelocity = FVector::ZeroVector;
}

void UEMMoverCurveWarpingFallingMode::SimulationTick_Implementation(const FSimulationTickParams& params, FMoverTickEndData& outputState)
{
	UMoverComponent* moverComponent = GetMoverComponent();
	const FMoverTickStartData& startState = params.StartState;
	USceneComponent* updatedComponent = params.MovingComps.UpdatedComponent.Get();
	FProposedMove proposedMove = params.ProposedMove;

	if (!IsValid(updatedComponent))
	{
		return;
	}

	const FCharacterDefaultInputs* characterInputs = startState.InputCmd.InputCollection.FindDataByType<FCharacterDefaultInputs>();
	const FMoverDefaultSyncState* startingSyncState = startState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(startingSyncState);

	FMoverDefaultSyncState& outputSyncState = outputState.SyncState.SyncStateCollection.FindOrAddMutableDataByType<FMoverDefaultSyncState>();

	const float deltaSeconds = params.TimeStep.StepMs * 0.001f;
	float pctTimeApplied = 0.f;

	FMovementRecord moveRecord;
	moveRecord.SetDeltaSeconds(deltaSeconds);
	
	UMoverBlackboard* simBlackboard = moverComponent->GetSimBlackboard_Mutable();

	simBlackboard->Invalidate(CommonBlackboard::LastFloorResult);
	simBlackboard->Invalidate(CommonBlackboard::LastFoundDynamicMovementBase);

	outputSyncState.MoveDirectionIntent = (proposedMove.bHasDirIntent ? proposedMove.DirectionIntent : FVector::ZeroVector);

	const FRotator startingOrient = startingSyncState->GetOrientation_WorldSpace();
	const FRotator targetOrient = UMovementUtils::ApplyAngularVelocityToRotator(startingOrient, proposedMove.AngularVelocityDegrees, deltaSeconds);
	
	const FVector upDirection = moverComponent->GetUpDirection();

	FFloorCheckSettings floorCheckSettings;
	floorCheckSettings.FloorSweepDistance = CommonLegacySettings->FloorSweepDistance;
	floorCheckSettings.MaxWalkSlopeCosine = CommonLegacySettings->MaxWalkSlopeCosine;
	floorCheckSettings.bUseFlatBaseForFloorChecks = CommonLegacySettings->bUseFlatBaseForFloorChecks;
	floorCheckSettings.PerchRadiusThreshold = CommonLegacySettings->PerchRadiusThreshold;

	FVector moveDelta;;
	bool bIsCurveWarpingComplated = GetMoveDeltaFallingDelegate.Execute(deltaSeconds, moveDelta);

	moveDelta += proposedMove.LinearVelocity * deltaSeconds;

	FHitResult hitResult(1.f);
	FQuat targetOrientQuat = targetOrient.Quaternion();

	if (CommonLegacySettings->bShouldRemainVertical)
	{
		targetOrientQuat = FRotationMatrix::MakeFromZX(upDirection, targetOrientQuat.GetForwardVector()).ToQuat();
	}

	UMovementUtils::TrySafeMoveUpdatedComponent(params.MovingComps, moveDelta, targetOrientQuat, true, hitResult, ETeleportType::None, moveRecord);

	FFloorCheckResult landingFloor;
	if (hitResult.IsValidBlockingHit() && updatedComponent)
	{
		pctTimeApplied += hitResult.Time * (1.f - pctTimeApplied);

		if (UAirMovementUtils::IsValidLandingSpot(params.MovingComps, updatedComponent->GetComponentLocation(), hitResult, floorCheckSettings, OUT landingFloor))
		{
			UGroundMovementUtils::TryMoveToKeepMinHeightAboveFloor(moverComponent, landingFloor, CommonLegacySettings->MaxWalkSlopeCosine, moveRecord);
			CaptureFinalState(updatedComponent, *startingSyncState, landingFloor, deltaSeconds, deltaSeconds * pctTimeApplied, proposedMove.AngularVelocityDegrees, outputSyncState, outputState, moveRecord);
			return;
		}
		
		landingFloor.HitResult = hitResult;
		simBlackboard->Set(CommonBlackboard::LastFloorResult, landingFloor);
		
		FMoverOnImpactParams ImpactParams(DefaultModeNames::Falling, hitResult, moveDelta);
		moverComponent->HandleImpact(ImpactParams);

		UAirMovementUtils::TryMoveToFallAlongSurface(params.MovingComps, moveDelta,
			(1.f - hitResult.Time), targetOrientQuat, hitResult.Normal, hitResult, true,
			floorCheckSettings, landingFloor, moveRecord);

		pctTimeApplied += hitResult.Time * (1.f - pctTimeApplied);

		if (landingFloor.IsWalkableFloor())
		{
			UGroundMovementUtils::TryMoveToKeepMinHeightAboveFloor(moverComponent, landingFloor, CommonLegacySettings->MaxWalkSlopeCosine, moveRecord);
			CaptureFinalState(updatedComponent, *startingSyncState, landingFloor, deltaSeconds, deltaSeconds * pctTimeApplied, proposedMove.AngularVelocityDegrees, outputSyncState, outputState, moveRecord);
			return;
		}
	}
	else
	{
		pctTimeApplied = 1.f;
	}
	
	CaptureFinalState(updatedComponent, *startingSyncState, landingFloor, deltaSeconds, deltaSeconds * pctTimeApplied, proposedMove.AngularVelocityDegrees, outputSyncState, outputState, moveRecord);

	if (bIsCurveWarpingComplated && outputState.MovementEndState.NextModeName == NAME_None)
	{
		outputState.MovementEndState.NextModeName = TEXT("Falling");
	}
}

void UEMMoverCurveWarpingFallingMode::Activate(const FMoverEventContext& context, FName prevModeName,
	const FMoverSimContext& simContext, const FMoverTickStartData& startState, FMoverSyncState* outSyncState, FMoverAuxStateContext* outAuxState)
{
	Super::Activate(context, prevModeName, simContext, startState, outSyncState, outAuxState);
}

void UEMMoverCurveWarpingFallingMode::ProcessLanded(const FFloorCheckResult& floorResult, FVector location, FVector& velocity, FRelativeBaseInfo& baseInfo, FMoverTickEndData& tickEndData) const
{
	const UMoverComponent* moverComp = GetMoverComponent();
	UMoverBlackboard* simBlackboard = moverComp->GetSimBlackboard_Mutable();

	const FVector upDir = moverComp->GetUpDirection();

	FName nextMovementMode = NAME_None; 
	if (floorResult.IsWalkableFloor())
	{
		if (bCancelVerticalSpeedOnLanding)
		{
			const FPlane MovementPlane(FVector::ZeroVector, upDir);
			velocity = UMovementUtils::ConstrainToPlane(velocity, MovementPlane, false);
		}
		else
		{
			velocity = FVector::VectorPlaneProject(velocity, floorResult.HitResult.Normal);
		}
		
		nextMovementMode = CommonLegacySettings->GroundMovementModeName;
		simBlackboard->Set(CommonBlackboard::LastFloorResult, floorResult);

		if (UBasedMovementUtils::IsADynamicBase(floorResult.HitResult.GetComponent()))
		{
			baseInfo.SetFromFloorResult(floorResult, location, upDir);
		}
	}
	if (!nextMovementMode.IsNone())
	{
		tickEndData.MovementEndState.NextModeName = nextMovementMode;
		OnLanded.Broadcast(nextMovementMode, floorResult.HitResult);
	}
}