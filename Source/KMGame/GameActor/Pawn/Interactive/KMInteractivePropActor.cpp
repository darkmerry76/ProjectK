#include "KMInteractivePropActor.h"

AKMInteractivePropActor::AKMInteractivePropActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMInteractivePropActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMInteractivePropActor::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void AKMInteractivePropActor::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	Super::OnImpact(skillEffectInstance, hitClosestPoint, hitTag);
}

void AKMInteractivePropActor::OnDeath()
{
	Super::OnDeath();
}

FBoxSphereBounds AKMInteractivePropActor::GetMasterBounds() const
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