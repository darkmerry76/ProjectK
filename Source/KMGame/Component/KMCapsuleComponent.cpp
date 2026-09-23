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
	CapsuleRadius = OriginCapsuleRadius;
	CapsuleHalfHeight = OriginCapsuleHalfHeight;
}