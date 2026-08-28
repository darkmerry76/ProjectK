#include "KMPawnMovementComponent.h"

UKMPawnMovementComponent::UKMPawnMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}