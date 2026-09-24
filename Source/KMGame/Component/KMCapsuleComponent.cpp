#include "KMCapsuleComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMCapsuleComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMCapsuleComponent::UKMCapsuleComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCapsuleComponent::BeginPlay()
{
	Super::BeginPlay();

	OriginCapsuleRadius = CapsuleRadius;
	OriginCapsuleHalfHeight = CapsuleHalfHeight;
}

void UKMCapsuleComponent::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void UKMCapsuleComponent::RevertOrigin()
{
	FVector offsetDelta(0.f, 0.f, OriginCapsuleHalfHeight - CapsuleHalfHeight);
	
	SetCapsuleRadius(OriginCapsuleRadius);
	SetCapsuleHalfHeight(OriginCapsuleHalfHeight);
	AddWorldOffset(offsetDelta);
	UpdateComponentToWorld();
}