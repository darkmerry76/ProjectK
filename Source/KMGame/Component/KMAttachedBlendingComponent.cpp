#include "KMAttachedBlendingComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAttachedBlendingComponent::UKMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, const FTransform& offsetTransform, float newDuration)
{
	AttachedParentComponent = newParentComponent;
	
	AttachedSocketName = newAttachSocketName;
	
	Duration = newDuration;
	BlendElipsedTime = 0.f;

	StartWorldTransform = startWorldTransform;
	OffsetTransform = offsetTransform;
	UpdateBlending(0.f);
}

void UKMAttachedBlendingComponent::StopBlending()
{
	AttachedParentComponent = nullptr;
}

void UKMAttachedBlendingComponent::UpdateBlending(float deltaTime)
{
	if (AttachedParentComponent.IsValid())
	{
		float alpha = 1.f;
		if (Duration > 0.f)
		{
			alpha = FMath::Clamp(BlendElipsedTime / Duration, 0.f, 1.f);
		}

		FTransform socketTransform = AttachedParentComponent->GetSocketTransform(AttachedSocketName,RTS_World);
		FTransform targetTransform = OffsetTransform * socketTransform;

		FTransform worldTransform;
		worldTransform.Blend(StartWorldTransform,targetTransform, alpha);
		SetWorldTransform(worldTransform);
	}

	BlendElipsedTime += deltaTime;
}

void UKMAttachedBlendingComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	UpdateBlending(deltaTime);
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
}