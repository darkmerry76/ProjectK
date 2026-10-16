#include "KMPawnMovementComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"

UKMPawnMovementComponent::UKMPawnMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

float UKMPawnMovementComponent::GetScaledRadius() const
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner());
	if (!ownerPawnInterface)
	{
		return 0.f;
	}

	return ownerPawnInterface->GetScaledRadius_Implementation();
}

float UKMPawnMovementComponent::GetScaledHalfHeight() const
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner());
	if (!ownerPawnInterface)
	{
		return 0.f;
	}

	return ownerPawnInterface->GetScaledHalfHeight_Implementation();
}

UPrimitiveComponent* UKMPawnMovementComponent::GetMovementBase() const
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner());
	if (!ownerPawnInterface)
	{
		return nullptr;
	}
	return ownerPawnInterface->GetMovementShapeComponent();
}

bool UKMPawnMovementComponent::IsValidLandingSpot(const FVector& location, const FHitResult& hitResult) const
{
	if (!hitResult.bBlockingHit)
	{
		return false;
	}
	if (!hitResult.bStartPenetrating)
	{
		IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetOwner());
		if (!pawnInterface)
		{
			return false;
		}
		
		if (!IsWalkable(hitResult))
		{
			return false;
		}

		float pawnRadius = GetScaledRadius();
		float pawnHalfHeight = GetScaledHalfHeight();

		const float lowerHemisphereZ = GetGravitySpaceZ(hitResult.Location) - pawnHalfHeight + pawnRadius;
		if (GetGravitySpaceZ(hitResult.ImpactPoint) >= lowerHemisphereZ)
		{
			return false;
		}
		
		if (!IsWithinEdgeTolerance(hitResult.Location, hitResult.ImpactPoint, pawnRadius))
		{
			return false;
		}
	}
	else
	{
		if (GetGravitySpaceZ(hitResult.Normal) < UE_KINDA_SMALL_NUMBER)
		{
			return false;
		}
	}

	FFindFloorResult floorResult;
	FindFloor(location, floorResult, false, &hitResult);

	if (!floorResult.IsWalkableFloor())
	{
		return false;
	}

	return true;
}

bool UKMPawnMovementComponent::CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations)
{
	return Super::CustomMovement(movementMode, adjusted, deltaTime, iterations);
}

bool UKMPawnMovementComponent::CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations)
{
	FHitResult hitResult;
	FVector finalAdjusted = FVector(adjusted.X, adjusted.Y, adjusted.Z);
	SafeMoveUpdatedComponent(finalAdjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);
	if (hitResult.bBlockingHit)
	{
		FVector gravDir = FVector(0.f, 0.f, -1.f); 
        
		SlideAlongSurface(finalAdjusted, 1.f - hitResult.Time, hitResult.Normal, hitResult, true);
	}
	return true;
}

bool UKMPawnMovementComponent::CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations)
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner());
	if (!ownerPawnInterface)
	{
		return false;
	}

	UKMGameObjectInstance* ownerGameObjectInstance = ownerPawnInterface->GetGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return false;
	}

	bool bIgnoreLanded = ownerGameObjectInstance->HasGameplayTag(FKMGameplayTagName::Event_Blow_IgnoreLanded_Tag);

	float remainingTime = deltaTime;

	if (remainingTime < MIN_TICK_TIME)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("CustomMovementFalling SKIP DeltaTime=%0.9f Adjusted=%s"),
			deltaTime,
			*adjusted.ToString());
	}

	int32 Iterations = 0;
	while( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) )
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		FVector finalAdjusted = adjusted * (timeTick / deltaTime);
		FHitResult hitResult;
		SafeMoveUpdatedComponent(finalAdjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);

		float subTimeTickRemaining = deltaTime * (1.f - hitResult.Time);
		float lastMoveTimeSlice = deltaTime;
		if (hitResult.bBlockingHit && !bIgnoreLanded)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
			{
				ProcessLanded(hitResult, timeTick, Iterations);
				return false;
			}
			else
			{
				if (!hitResult.bStartPenetrating && ShouldCheckForValidLandingSpot(deltaTime, finalAdjusted, hitResult))
				{
					const FVector pawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult floorResult;
					FindFloor(pawnLocation, floorResult, false);

					if (!floorResult.bLineTrace && finalAdjusted.Z < 0.f && floorResult.IsWalkableFloor() && IsValidLandingSpot(pawnLocation, floorResult.HitResult))
					{
						ProcessLanded(floorResult.HitResult, timeTick, Iterations);
						return false;
					}
				}
				
				if (hitResult.IsValidBlockingHit())
				{
					if (hitResult.ImpactNormal.Z < 0.3f && hitResult.ImpactNormal.Z > -0.5f)
					{
						FVector slideDelta = ComputeSlideVector(
							finalAdjusted,
							1.f - hitResult.Time,
							hitResult.Normal,
							hitResult
						);

						const float moveDistance = finalAdjusted.Size();
						const float slideDistance = slideDelta.Size();

						if (slideDistance < moveDistance * 0.3f)
						{
							ProcessWallHit(hitResult);
						}
					}
					else if (hitResult.ImpactNormal.Z < -0.5f)
					{
						ProcessCeilingHit(hitResult);
					}
				}
				
				const FVector oldHitNormal = hitResult.Normal;
				const FVector oldHitImpactNormal = hitResult.ImpactNormal;				
				FVector delta = ComputeSlideVector(finalAdjusted, 1.f - hitResult.Time, oldHitNormal, hitResult);
				if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && (delta | finalAdjusted) > 0.f)
				{
					SafeMoveUpdatedComponent( delta, UpdatedComponent->GetComponentRotation(), true, hitResult);
					if (hitResult.bBlockingHit)
					{
						subTimeTickRemaining = subTimeTickRemaining * (1.f - hitResult.Time);

						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
						{
							ProcessLanded(hitResult, timeTick, Iterations);
							return false;
						}
						lastMoveTimeSlice = subTimeTickRemaining;
						HandleImpact(hitResult, lastMoveTimeSlice, delta);
						TwoWallAdjust(delta, hitResult, oldHitNormal);

						bool bDitch = ( (GetGravitySpaceZ(oldHitImpactNormal) > 0.f) &&
							(GetGravitySpaceZ(hitResult.ImpactNormal) > 0.f) && (FMath::Abs(GetGravitySpaceZ(delta)) <= UE_KINDA_SMALL_NUMBER) && ((hitResult.ImpactNormal | oldHitImpactNormal) < 0.f) );

						SafeMoveUpdatedComponent( delta, UpdatedComponent->GetComponentRotation(), true, hitResult);
						if (hitResult.Time == 0.f )
						{
							FVector sideDelta = ProjectToGravityFloor(oldHitNormal + hitResult.ImpactNormal).GetSafeNormal();
							if(sideDelta.IsNearlyZero())
							{
								if (HasCustomGravity())
								{
									const FVector gravityRelativeHitNormal = RotateWorldToGravity(oldHitNormal);
									sideDelta = RotateGravityToWorld(FVector(gravityRelativeHitNormal.Y, -gravityRelativeHitNormal.X, 0.f)).GetSafeNormal();
								}
								else
								{
									sideDelta = FVector(oldHitNormal.Y, -oldHitNormal.X, 0).GetSafeNormal();	
								}
							}
							SafeMoveUpdatedComponent(sideDelta, UpdatedComponent->GetComponentRotation(), true, hitResult);
						}
						if(bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult) || hitResult.Time == 0.f)
						{
							ProcessLanded(hitResult, timeTick, Iterations);
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

bool UKMPawnMovementComponent::CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations)
{
	FHitResult hitResult;
	SafeMoveUpdatedComponent(adjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);
	
	if (hitResult.IsValidBlockingHit())
	{
		FVector moveDirection = FVector(adjusted.X, adjusted.Y, 0.f).GetSafeNormal();
		FVector impactNormal = FVector(hitResult.ImpactNormal.X, hitResult.ImpactNormal.Y, 0.f).GetSafeNormal();

		if (FVector::DotProduct(moveDirection, impactNormal) < -UE_KINDA_SMALL_NUMBER)
		{
			BlockHitResult = hitResult;
		}

		FVector gravDir = FVector(0.f, 0.f, -1.f); 
        
		if (!StepUp(gravDir, adjusted, hitResult, nullptr))
		{
			SlideAlongSurface(adjusted, 1.f - hitResult.Time, hitResult.Normal, hitResult, true);
		}
	}
	return true;
}

float UKMPawnMovementComponent::GetSimulationTimeStep(float RemainingTime, int32 Iterations) const
{
	if (RemainingTime > MaxSimulationTimeStep)
	{
		if (Iterations < MaxSimulationIterations)
		{
			RemainingTime = FMath::Min(MaxSimulationTimeStep, RemainingTime * 0.5f);
		}
	}
	return FMath::Max(MIN_TICK_TIME, RemainingTime);
}

void UKMPawnMovementComponent::ProcessLanded(const FHitResult& hitResult, float remainingTime, int32 iterations)
{
	Super::ProcessLanded(hitResult, remainingTime, iterations);

	if (IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwner()))
	{
		ownerPawnInterface->LandHit(hitResult);
	}
}