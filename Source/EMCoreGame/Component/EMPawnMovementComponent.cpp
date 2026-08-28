#include "EMPawnMovementComponent.h"
#include "EMCurveWarpingComponent.h"

UEMPawnMovementComponent::UEMPawnMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UEMPawnMovementComponent::BindCurveWarpingComponent(UEMCurveWarpingComponent* newCurveWarpingComponent)
{
}

void UEMPawnMovementComponent::UnbindCurveWarpingComponent(UEMCurveWarpingComponent* newCurveWarpingComponent)
{
}

void UEMPawnMovementComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	CustomMovementDelegate.Broadcast(deltaTime, 1);
}

bool UEMPawnMovementComponent::CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations)
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