#include "EMAttachedBlendingComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMAttachedBlendingComponent::UEMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, const FTransform& offsetTransform, float newDuration)
{
	PrimaryComponentTick.bCanEverTick = true;
	AttachedParentComponent = newParentComponent;
	
	AttachedSocketName = newAttachSocketName;
	
	Duration = newDuration;
	BlendElipsedTime = 0.f;

	StartWorldTransform = startWorldTransform;
	OffsetTransform = offsetTransform;
	UpdateBlending(0.f);
}

void UEMAttachedBlendingComponent::StopBlending()
{
	AttachedParentComponent = nullptr;
	PrimaryComponentTick.bCanEverTick = false;
}

void UEMAttachedBlendingComponent::UpdateBlending(float deltaTime)
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

void UEMAttachedBlendingComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	UpdateBlending(deltaTime);
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
}