#include "KMAttachedBlendingComponent.h"
#include "KMSkeletalMeshComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAttachedBlendingComponent::UKMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, float newDuration)
{
	Super::StartBlending(newParentComponent, newAttachSocketName, startWorldTransform, newDuration);

	if (UKMSkeletalMeshComponent* skeletalMeshComponent = Cast<UKMSkeletalMeshComponent>(newParentComponent))
	{
		skeletalMeshComponent->AttachBlendingComponent(this);
	}
}