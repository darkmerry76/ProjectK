#include "EMPawnMovementComponent.h"
#include "EMCurveWarpingComponent.h"

const float UEMPawnMovementComponent::MIN_TICK_TIME = 1e-6f;
const float UEMPawnMovementComponent::MIN_FLOOR_DIST = 1.9f;
const float UEMPawnMovementComponent::MAX_FLOOR_DIST = 2.4f;
const float UEMPawnMovementComponent::SWEEP_EDGE_REJECT_DISTANCE = 0.15f;
const float UEMPawnMovementComponent::MAX_STEP_SIDE_Z = 0.08f;

struct FKMScopedCapsuleMovementUpdate : public FScopedMovementUpdate
{
	typedef FScopedMovementUpdate Super;

	FKMScopedCapsuleMovementUpdate(USceneComponent* UpdatedComponent, bool bEnabled)
	: Super(bEnabled ? UpdatedComponent : nullptr, EScopedUpdate::DeferredUpdates)
	{
	}
};

UEMPawnMovementComponent::UEMPawnMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

float UEMPawnMovementComponent::GetScaledRadius() const
{
	return 0.f;
}

float UEMPawnMovementComponent::GetScaledHalfHeight() const
{
	return 0.f;
}

UPrimitiveComponent* UEMPawnMovementComponent::GetMovementBase() const
{
	check(0);
	return nullptr;	
}

void UEMPawnMovementComponent::BindCurveWarpingComponent(UEMCurveWarpingComponent* newCurveWarpingComponent)
{
}

void UEMPawnMovementComponent::UnbindCurveWarpingComponent(UEMCurveWarpingComponent* newCurveWarpingComponent)
{
}

bool UEMPawnMovementComponent::CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations)
{
	bool bResult = true;

	float deltaPerSeconds = (FMath::IsNearlyZero(deltaTime) ? 0.f : (1.f / deltaTime));
	
	if (movementMode == EEMCustomMovementMode::CMODE_Walking)
	{
		bResult = CustomMovementWalking(adjusted, deltaTime, iterations);
	}
	else if (movementMode == EEMCustomMovementMode::CMODE_Falling)
	{
		Velocity.Z = adjusted.Z * deltaPerSeconds;
		bResult = CustomMovementFalling(adjusted, deltaTime, iterations);
	}
	else if (movementMode == EEMCustomMovementMode::CMODE_Flying)
	{
		bResult = CustomMovementFlying(adjusted, deltaTime, iterations);
	}
	
	return bResult;
}

bool UEMPawnMovementComponent::CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations)
{
	return true;
}

bool UEMPawnMovementComponent::CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations)
{
	return true;
}

bool UEMPawnMovementComponent::CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations)
{
	return true;
}

void UEMPawnMovementComponent::ProcessLanded(const FHitResult& hitResult, float remainingTime, int32 iterations)
{
}

void UEMPawnMovementComponent::ProcessWallHit(const FHitResult hitResult)
{
}

void UEMPawnMovementComponent::ProcessCeilingHit(const FHitResult hitResult)
{
}

bool UEMPawnMovementComponent::IsAir() const
{
	return false;
}

bool UEMPawnMovementComponent::HasValidData() const
{
	return IsValid(UpdatedComponent);
}

bool UEMPawnMovementComponent::IsDynamicBase() const
{
	UPrimitiveComponent* movementBase = GetMovementBase();
	return (IsValid(movementBase) && movementBase->Mobility == EComponentMobility::Movable);
}

float UEMPawnMovementComponent::GetPerchRadiusThreshold() const
{
	return FMath::Max(0.f, PerchRadiusThreshold);
}

float UEMPawnMovementComponent::GetValidPerchRadius() const
{
	const float pawnRadius = GetScaledRadius();
	return FMath::Clamp(pawnRadius - GetPerchRadiusThreshold(), 0.11f, pawnRadius);
}

bool UEMPawnMovementComponent::ComputePerchResult(const float testRadius, const FHitResult& hitResult, const float maxFloorDist, FFindFloorResult& outPerchFloorResult) const
{
	if (maxFloorDist <= 0.f)
	{
		return false;
	}

	float pawnRadius = GetScaledRadius();
	float pawnHalfHeight = GetScaledHalfHeight();

	const FVector pawnLocation = (bUseFlatBaseForFloorChecks ? hitResult.TraceStart : hitResult.Location);

	const float hitAboveBase = FMath::Max<float>(0.f, GetGravitySpaceZ(hitResult.ImpactPoint - pawnLocation) + pawnHalfHeight);
	const float perchLineDist = FMath::Max(0.f, maxFloorDist - hitAboveBase);
	const float perchSweepDist = FMath::Max(0.f, maxFloorDist);

	const float actualSweepDist = perchSweepDist + pawnRadius;
	ComputeFloorDist(pawnLocation, perchLineDist, actualSweepDist, outPerchFloorResult, testRadius);

	if (!outPerchFloorResult.IsWalkableFloor())
	{
		return false;
	}
	else if (hitAboveBase + outPerchFloorResult.FloorDist > maxFloorDist)
	{
		outPerchFloorResult.bWalkableFloor = false;
		return false;
	}
	
	return true;
}

bool UEMPawnMovementComponent::ShouldComputePerchResult(const FHitResult& hitResult, bool bCheckRadius) const
{
	if (!hitResult.IsValidBlockingHit())
	{
		return false;
	}

	if (GetPerchRadiusThreshold() <= SWEEP_EDGE_REJECT_DISTANCE)
	{
		return false;
	}

	if (bCheckRadius)
	{
		const float distFromCenterSq = ProjectToGravityFloor(hitResult.ImpactPoint - hitResult.Location).SizeSquared();
		const float standOnEdgeRadius = GetValidPerchRadius();
		if (distFromCenterSq <= FMath::Square(standOnEdgeRadius))
		{
			return false;
		}
	}
	
	return true;
}


bool UEMPawnMovementComponent::IsWalkable(const FHitResult& hitResult) const
{
	if (!hitResult.IsValidBlockingHit())
	{
		return false;
	}

	const FVector::FReal impactNormalZ = GetGravitySpaceZ(hitResult.ImpactNormal);
	if (impactNormalZ < UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

	float testWalkableZ = WalkableFloorZ;

	if (const UPrimitiveComponent* hitComponent = hitResult.Component.Get())
	{
		const FWalkableSlopeOverride& slopeOverride = hitComponent->GetWalkableSlopeOverride();
		testWalkableZ = slopeOverride.ModifyWalkableFloorZ(testWalkableZ);
	}

	if (impactNormalZ < testWalkableZ)
	{
		return false;
	}

	return true;
}

bool UEMPawnMovementComponent::IsWithinEdgeTolerance(const FVector& pawnLocation, const FVector& testImpactPoint, const float radius) const
{
	const float distFromCenterSq = ProjectToGravityFloor(testImpactPoint - pawnLocation).SizeSquared();
	const float reducedRadiusSq = FMath::Square(FMath::Max(SWEEP_EDGE_REJECT_DISTANCE + UE_KINDA_SMALL_NUMBER, radius - SWEEP_EDGE_REJECT_DISTANCE));
	return distFromCenterSq < reducedRadiusSq;
}

bool UEMPawnMovementComponent::FloorSweepTest(FHitResult& outHitResult, const FVector& start, const FVector& end,
	ECollisionChannel traceChannel,
	const struct FCollisionShape& collisionShape,
	const struct FCollisionQueryParams& params,
	const struct FCollisionResponseParams& responseParam) const
{
	bool bBlockingHit = false;

	if (!bUseFlatBaseForFloorChecks)
	{
		bBlockingHit = GetWorld()->SweepSingleByChannel(outHitResult, start, end, GetWorldToGravityTransform(), traceChannel, collisionShape, params, responseParam);
	}
	else
	{
		const float pawnRdius = GetScaledRadius();
		const float pawnHalfHeight = GetScaledHalfHeight();
		const FCollisionShape boxShape = FCollisionShape::MakeBox(FVector(pawnRdius * 0.707f, pawnRdius * 0.707f, pawnHalfHeight));

		bBlockingHit = GetWorld()->SweepSingleByChannel(outHitResult, start, end, FQuat(GetGravityDirection(), UE_PI * 0.25f), traceChannel, boxShape, params, responseParam);

		if (!bBlockingHit)
		{
			outHitResult.Reset(1.f, false);
			bBlockingHit = GetWorld()->SweepSingleByChannel(outHitResult, start, end, GetWorldToGravityTransform(), traceChannel, boxShape, params, responseParam);
		}
	}

	return bBlockingHit;
}

void UEMPawnMovementComponent::ComputeFloorDist(const FVector& pawnLocation, float lineDistance, float sweepDistance, FFindFloorResult& outFloorResult, float sweepRadius, const FHitResult* downwardSweepResult) const
{
	outFloorResult.Clear();

	float pawnRadius = GetScaledRadius();
	float pawnHalfHeight = GetScaledHalfHeight();

	bool bSkipSweep = false;
	if (downwardSweepResult && downwardSweepResult->IsValidBlockingHit())
	{
		const bool bIsDownward = GetGravitySpaceZ(downwardSweepResult->TraceStart - downwardSweepResult->TraceEnd) > 0;
		const bool bIsVertical = ProjectToGravityFloor(downwardSweepResult->TraceStart - downwardSweepResult->TraceEnd).SizeSquared() <= UE_KINDA_SMALL_NUMBER;
		if (bIsDownward && bIsVertical)
		{
			if (IsWithinEdgeTolerance(downwardSweepResult->Location, downwardSweepResult->ImpactPoint, pawnRadius))
			{
				bSkipSweep = true;

				const bool bIsWalkable = IsWalkable(*downwardSweepResult);
				const float floorDist = GetGravitySpaceZ(pawnLocation - downwardSweepResult->Location);
				outFloorResult.SetFromSweep(*downwardSweepResult, floorDist, bIsWalkable);
				if (bIsWalkable)
				{
					return;
				}
			}
		}
	}

	if (sweepDistance < lineDistance)
	{
		ensure(sweepDistance >= lineDistance);
		return;
	}

	bool bBlockingHit = false;

	FCollisionQueryParams queryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, GetOwner());
	queryParams.bTraceIntoSubComponents = true;
	queryParams.bReplaceHitWithSubComponents = false;

	FCollisionResponseParams responseParam;
	InitCollisionParams(queryParams, responseParam);
	const ECollisionChannel collisionChannel = UpdatedComponent->GetCollisionObjectType();

	if (!bSkipSweep && sweepDistance > 0.f && sweepRadius > 0.f)
	{
		const float shrinkScale = 0.9f;
		const float shrinkScaleOverlap = 0.1f;
		float shrinkHeight = (pawnHalfHeight - pawnRadius) * (1.f - shrinkScale);
		float traceDist = sweepDistance + shrinkHeight;
		FCollisionShape collisionShape = FCollisionShape::MakeCapsule(sweepRadius, pawnHalfHeight - shrinkHeight);

		FHitResult hit(1.f);
		bBlockingHit = FloorSweepTest(hit, pawnLocation, pawnLocation + traceDist * GetGravityDirection(), collisionChannel, collisionShape, queryParams, responseParam);

		if (bBlockingHit)
		{
			if (hit.bStartPenetrating || !IsWithinEdgeTolerance(pawnLocation, hit.ImpactPoint, collisionShape.Capsule.Radius))
			{
				collisionShape.Capsule.Radius = FMath::Max(0.f, collisionShape.Capsule.Radius - SWEEP_EDGE_REJECT_DISTANCE - UE_KINDA_SMALL_NUMBER);
				if (!collisionShape.IsNearlyZero())
				{
					shrinkHeight = (pawnHalfHeight - pawnRadius) * (1.f - shrinkScaleOverlap);
					traceDist = sweepDistance + shrinkHeight;
					collisionShape.Capsule.HalfHeight = FMath::Max(pawnHalfHeight - shrinkHeight, collisionShape.Capsule.Radius);
					hit.Reset(1.f, false);

					bBlockingHit = FloorSweepTest(hit, pawnLocation, pawnLocation + traceDist * GetGravityDirection(), collisionChannel, collisionShape, queryParams, responseParam);
				}
			}

			const float maxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, pawnRadius);
			const float sweepResult = FMath::Max(-maxPenetrationAdjust, hit.Time * traceDist - shrinkHeight);

			outFloorResult.SetFromSweep(hit, sweepResult, false);
			if (hit.IsValidBlockingHit() && IsWalkable(hit))
			{		
				if (sweepResult <= sweepDistance)
				{
					outFloorResult.bWalkableFloor = true;
					return;
				}
			}
		}
	}

	if (!outFloorResult.bBlockingHit && !outFloorResult.HitResult.bStartPenetrating)
	{
		outFloorResult.FloorDist = sweepDistance;
		return;
	}

	if (lineDistance > 0.f)
	{
		const float shrinkHeight = pawnHalfHeight;
		const FVector lineTraceStart = pawnLocation;	
		const float traceDist = lineDistance + shrinkHeight;
		const FVector down = traceDist * GetGravityDirection(); 
		queryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult hit(1.f);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(hit, lineTraceStart, lineTraceStart + down, collisionChannel, queryParams, responseParam);

		if (bBlockingHit)
		{
			if (hit.Time > 0.f)
			{
				const float maxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, pawnRadius);
				const float lineResult = FMath::Max(-maxPenetrationAdjust, hit.Time * traceDist - shrinkHeight);

				outFloorResult.bBlockingHit = true;
				if (lineResult <= lineDistance && IsWalkable(hit))
				{
					outFloorResult.SetFromLineTrace(hit, outFloorResult.FloorDist, lineResult, true);
					return;
				}
			}
		}
	}
	
	outFloorResult.bWalkableFloor = false;
}

void UEMPawnMovementComponent::FindFloor(const FVector& pawnLocation, FFindFloorResult& outFloorResult, bool bCanUseCachedLocation, const FHitResult* downwardSweepResult) const
{
	if (!HasValidData() || !UpdatedComponent->IsQueryCollisionEnabled())
	{
		outFloorResult.Clear();
		return;
	}

	const float heightCheckAdjust = (IsMovingOnGround() ? MAX_FLOOR_DIST + UE_KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float floorSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + heightCheckAdjust);
	float floorLineTraceDist = floorSweepTraceDist;
	bool bNeedToValidateFloor = true;
	
	if (floorLineTraceDist > 0.f || floorSweepTraceDist > 0.f)
	{
		UEMPawnMovementComponent* mutableThis = const_cast<UEMPawnMovementComponent*>(this);
		if (bAlwaysCheckFloor || !bCanUseCachedLocation || bForceNextFloorCheck || bJustTeleported)
		{
			mutableThis->bForceNextFloorCheck = false;
			ComputeFloorDist(pawnLocation, floorLineTraceDist, floorSweepTraceDist, outFloorResult, GetScaledRadius(), downwardSweepResult);
		}
		else
		{
			UPrimitiveComponent* MovementBase = GetMovementBase();
			const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

			if (MovementBase)
			{
				mutableThis->bForceNextFloorCheck = !MovementBase->IsQueryCollisionEnabled() ||
					MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block || IsDynamicBase();
			}

			const bool IsActorBasePendingKill = BaseActor && !IsValid(BaseActor);

			if ( !bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase )
			{
				outFloorResult = CurrentFloor;
				bNeedToValidateFloor = false;
			}
			else
			{
				mutableThis->bForceNextFloorCheck = false;
				ComputeFloorDist(pawnLocation, floorLineTraceDist, floorSweepTraceDist, outFloorResult, GetScaledRadius(), downwardSweepResult);
			}
		}
	}

	if (bNeedToValidateFloor && outFloorResult.bBlockingHit && !outFloorResult.bLineTrace)
	{
		const bool bCheckRadius = true;
		if (ShouldComputePerchResult(outFloorResult.HitResult, bCheckRadius))
		{
			float maxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + heightCheckAdjust);
			if (IsMovingOnGround())
			{
				maxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
			}

			FFindFloorResult perchFloorResult;
			if (ComputePerchResult(GetValidPerchRadius(), outFloorResult.HitResult, maxPerchFloorDist, perchFloorResult))
			{
				const float avgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
				const float moveUpDist = (avgFloorDist - outFloorResult.FloorDist);
				if (moveUpDist + perchFloorResult.FloorDist >= maxPerchFloorDist)
				{
					outFloorResult.FloorDist = avgFloorDist;
				}

				if (!outFloorResult.bWalkableFloor)
				{
					outFloorResult.SetFromLineTrace(perchFloorResult.HitResult, outFloorResult.FloorDist, FMath::Max(outFloorResult.FloorDist, MIN_FLOOR_DIST), true);
				}
			}
			else
			{
				outFloorResult.bWalkableFloor = false;
			}
		}
	}
}

bool UEMPawnMovementComponent::ShouldCheckForValidLandingSpot(float deltaTime, const FVector& delta, const FHitResult& hitResult) const
{
	if (GetGravitySpaceZ(hitResult.Normal) > UE_KINDA_SMALL_NUMBER && !hitResult.Normal.Equals(hitResult.ImpactNormal))
	{
		const FVector pawnLocation = UpdatedComponent->GetComponentLocation();
		if (IsWithinEdgeTolerance(pawnLocation, hitResult.ImpactPoint, GetScaledRadius()))
		{						
			return true;
		}
	}

	return false;
}

bool UEMPawnMovementComponent::CanStepUp(const FHitResult& hitResult) const
{
	if (!hitResult.IsValidBlockingHit() || !HasValidData() || MovementMode == MOVE_Falling)
	{
		return false;
	}

	const UPrimitiveComponent* HitComponent = hitResult.Component.Get();
	if (!HitComponent)
	{
		return true;
	}
	APawn* ownerPawn = Cast<APawn>(GetOwner());

	if (!HitComponent->CanCharacterStepUp(ownerPawn))
	{
		return false;
	}
	
	if (!hitResult.HitObjectHandle.IsValid())
	{
		return true;
	}

	const AActor* HitActor = hitResult.HitObjectHandle.GetManagingActor();
	if (!HitActor->CanBeBaseForCharacter(ownerPawn))
	{
		return false;
	}

	return true;
}

bool UEMPawnMovementComponent::StepUp(const FVector& gravDir, const FVector& delta, const FHitResult &hitResult, FStepDownResult* outStepDownResult)
{
	if (!CanStepUp(hitResult) || MaxStepHeight <= 0.f)
	{
		return false;
	}

	const FVector oldLocation = UpdatedComponent->GetComponentLocation();
	
	float pawnRadius = GetScaledRadius();
	float pawnHalfHeight = GetScaledHalfHeight();

	const float initialImpactZ = hitResult.ImpactPoint | -gravDir;
	const float oldLocationZ = oldLocation | -gravDir;
	if (initialImpactZ > oldLocationZ + (pawnHalfHeight - pawnRadius))
	{
		return false;
	}

	if (gravDir.IsZero())
	{
		return false;
	}

	ensure(gravDir.IsNormalized());

	float stepTravelUpHeight = MaxStepHeight;
	float stepTravelDownHeight = stepTravelUpHeight;
	const float stepSideZ = -1.f * FVector::DotProduct(hitResult.ImpactNormal, gravDir);
	float pawnInitialFloorBaseZ = oldLocationZ - pawnHalfHeight;
	float pawnFloorPointZ = pawnInitialFloorBaseZ;

	if (IsMovingOnGround() && CurrentFloor.IsWalkableFloor())
	{
		const float floorDist = FMath::Max(0.f, CurrentFloor.GetDistanceToFloor());
		pawnInitialFloorBaseZ -= floorDist;
		stepTravelUpHeight = FMath::Max(stepTravelUpHeight - floorDist, 0.f);
		stepTravelDownHeight = (MaxStepHeight + MAX_FLOOR_DIST*2.f);

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(hitResult.Location, hitResult.ImpactPoint, pawnRadius);
		if (!CurrentFloor.bLineTrace && !bHitVerticalFace)
		{
			pawnFloorPointZ = CurrentFloor.HitResult.ImpactPoint | -gravDir;
		}
		else
		{
			pawnFloorPointZ -= CurrentFloor.FloorDist;
		}
	}

	if (initialImpactZ <= pawnInitialFloorBaseZ)
	{
		return false;
	}

	FKMScopedCapsuleMovementUpdate scopedStepUpMovement(UpdatedComponent, true);

	FHitResult sweepUpHit(1.f);
	const FQuat pawnRotation = UpdatedComponent->GetComponentQuat();
	MoveUpdatedComponent(-gravDir * stepTravelUpHeight, pawnRotation, true, &sweepUpHit);

	if (sweepUpHit.bStartPenetrating)
	{
		scopedStepUpMovement.RevertMove();
		return false;
	}

	FHitResult hit(1.f);
	MoveUpdatedComponent(delta, pawnRotation, true, &hit);

	if (hit.bBlockingHit)
	{
		if (hit.bStartPenetrating)
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}

		if (sweepUpHit.bBlockingHit && hit.bBlockingHit)
		{
			HandleImpact(sweepUpHit);
		}

		HandleImpact(hit);
		if (IsFalling())
		{
			return true;
		}

		const float forwardHitTime = hit.Time;
		const float forwardSlideAmount = SlideAlongSurface(delta, 1.f - hit.Time, hit.Normal, hit, true);
		
		if (IsFalling())
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}

		if (forwardHitTime == 0.f && forwardSlideAmount == 0.f)
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}
	}
	
	MoveUpdatedComponent(gravDir * stepTravelDownHeight, UpdatedComponent->GetComponentQuat(), true, &hit);

	if (hit.bStartPenetrating)
	{
		scopedStepUpMovement.RevertMove();
		return false;
	}

	FStepDownResult stepDownResult;
	if (hit.IsValidBlockingHit())
	{	
		const float deltaZ = (hit.ImpactPoint | -gravDir) - pawnFloorPointZ;
		if (deltaZ > MaxStepHeight)
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}

		if (!IsWalkable(hit))
		{
			const bool bNormalTowardsMe = (delta | hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe)
			{
				scopedStepUpMovement.RevertMove();
				return false;
			}

			if ((hit.Location | -gravDir) > oldLocationZ)
			{
				scopedStepUpMovement.RevertMove();
				return false;
			}
		}

		if (!IsWithinEdgeTolerance(hit.Location, hit.ImpactPoint, pawnRadius))
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}

		if (deltaZ > 0.f && !CanStepUp(hit))
		{
			scopedStepUpMovement.RevertMove();
			return false;
		}

		if (outStepDownResult)
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), stepDownResult.FloorResult, false, &hit);
			if ((hit.Location | -gravDir) > oldLocationZ)
			{
				if (!stepDownResult.FloorResult.bBlockingHit && stepSideZ < MAX_STEP_SIDE_Z)
				{
					scopedStepUpMovement.RevertMove();
					return false;
				}
			}

			stepDownResult.bComputedFloor = true;
		}
	}
	
	if (outStepDownResult)
	{
		*outStepDownResult = stepDownResult;
	}

	bJustTeleported |= !bMaintainHorizontalGroundVelocity;
	return true;
}

void UEMPawnMovementComponent::SetMovementMode(EMovementMode newMovementMode, uint8 newCustomMode)
{
	MovementMode = newMovementMode;
}

void UEMPawnMovementComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	if (IsActive())
	{
		CustomMovementDelegate.Broadcast(deltaTime, 1);

		if (MovementMode == MOVE_Falling)
		{
			CustomMovementFalling(Velocity * deltaTime, deltaTime, 1);
		}
	}
}
