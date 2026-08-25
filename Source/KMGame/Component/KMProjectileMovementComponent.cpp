#include "KMProjectileMovementComponent.h"
#include "MovieSceneTracksPropertyTypes.h"
#include "GameFramework/Actor.h"

UKMProjectileMovementComponent::UKMProjectileMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

FKMProjectileArrivalDelegate& UKMProjectileMovementComponent::GetProjectileArrivalDelegate()
{
	return OnProjectileArrival;
}

const FTransform& UKMProjectileMovementComponent::GetDeltaTransform() const
{
	return DeltaTransform;
}

FVector UKMProjectileMovementComponent::ComputeHomingAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	return Super::ComputeHomingAcceleration(InVelocity, DeltaTime);
}

void UKMProjectileMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector prevUpdatedLocation;
	if (IsValid(UpdatedComponent))
	{
		prevUpdatedLocation = UpdatedComponent->GetComponentLocation();
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(UpdatedComponent) && HomingTargetComponent.IsValid())
	{
		FVector nextUpdatedLocation = UpdatedComponent->GetComponentLocation();
		FVector targetLocation = HomingTargetComponent->GetComponentLocation();

		FVector ToTargetPrev = targetLocation - prevUpdatedLocation;
		FVector ToTargetNext = targetLocation - nextUpdatedLocation;

		DeltaTransform.SetLocation(nextUpdatedLocation - targetLocation);
		DeltaTransform.SetRotation((nextUpdatedLocation - targetLocation).GetSafeNormal().ToOrientationQuat());

		if (FVector::DotProduct(ToTargetPrev, ToTargetNext) <= 0.f)
		{
			bool isPendingKill = IsGarbageEliminationEnabled();
			if (!isPendingKill)
			{
				OnProjectileArrival.Broadcast(this);
				if (IsArrivalDestroyActor)
				{
					GetOwner()->Destroy();
				}
			}
		}
	}
}