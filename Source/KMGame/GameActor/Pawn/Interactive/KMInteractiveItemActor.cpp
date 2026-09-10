#include "KMInteractiveItemActor.h"

AKMInteractiveItemActor::AKMInteractiveItemActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMInteractiveItemActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMInteractiveItemActor::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void AKMInteractiveItemActor::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	Super::OnImpact(skillEffectInstance, hitClosestPoint, hitTag);
}

void AKMInteractiveItemActor::OnDeath()
{
	Super::OnDeath();
}

FBoxSphereBounds AKMInteractiveItemActor::GetMasterBounds() const
{
	FBox bounds(ForceInit);

	TArray<UPrimitiveComponent*> primitiveComponents;
	GetComponents<UPrimitiveComponent>(primitiveComponents);

	for (UPrimitiveComponent* primitiveComponent : primitiveComponents)
	{
		if (!IsValid(primitiveComponent))
		{
			continue;
		}

		bounds += primitiveComponent->Bounds.GetBox();
	}
	return bounds;
}