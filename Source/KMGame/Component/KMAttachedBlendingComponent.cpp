#include "KMAttachedBlendingComponent.h"
#include "KMSkeletalMeshComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAttachedBlendingComponent::UKMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& targetWorldTransform, float newDuration)
{
	if (UKMSkeletalMeshComponent* parentSkeletalMeshComponent = Cast<UKMSkeletalMeshComponent>(newParentComponent))
	{
		parentSkeletalMeshComponent->AttachBlendingComponent(this);
	}

	Super::StartBlending(newParentComponent, newAttachSocketName, targetWorldTransform, newDuration);
}

void UKMAttachedBlendingComponent::StopBlending()
{
	if (UKMSkeletalMeshComponent* parentSkeletalMeshComponent = Cast<UKMSkeletalMeshComponent>(AttachedParentComponent.Get()))
	{
		parentSkeletalMeshComponent->DetachBlendingComponent(this);
	}
	
	Super::StopBlending();
}
