#include "EMAttachedBlendingComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMAttachedBlendingComponent::UEMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, float newDuration)
{
	PrimaryComponentTick.bCanEverTick = true;
	AttachedParentComponent = newParentComponent;
	
	AttachedSocketName = newAttachSocketName;
	
	Duration = newDuration;
	BlendElipsedTime = 0.f;

	StartWorldTransform = startWorldTransform;

	UpdateBlending();
}

void UEMAttachedBlendingComponent::StopBlending()
{
	AttachedParentComponent = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UEMAttachedBlendingComponent::UpdateBlending()
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
		SetWorldLocation(worldTransform.GetLocation());
		SetWorldRotation(worldTransform.GetRotation());
	}
}

void UEMAttachedBlendingComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
	BlendElipsedTime += deltaTime;
}