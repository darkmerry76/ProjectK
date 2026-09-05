#include "EMAttachedBlendingComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMAttachedBlendingComponent::UEMAttachedBlendingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

const FTransform& UEMAttachedBlendingComponent::GetOffsetTransform() const
{
	return OffsetTransform;
}

void UEMAttachedBlendingComponent::StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& newTargetWorldTransform, float newDuration)
{
	AttachedParentComponent = newParentComponent;
	AttachedSocketName = newAttachSocketName;
	
	Duration = newDuration;
	BlendElipsedTime = 0.f;

	TargetWorldTransform = newTargetWorldTransform;

	UpdateBlending();
}

void UEMAttachedBlendingComponent::StopBlending()
{
	UpdateBlending();
	
	AttachedParentComponent = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UEMAttachedBlendingComponent::UpdateBlending()
{
	if (AttachedParentComponent.IsValid())
	{
		float alpha = 1.f;
		float absDuration = FMath::Abs(Duration);
		if (absDuration > 0.f)
		{
			alpha = FMath::Clamp(BlendElipsedTime / absDuration, 0.f, 1.f);
			if (Duration < 0.f)
			{
				alpha = 1.f - alpha;
			}
		}
		
		FTransform socketTransform = AttachedParentComponent->GetSocketTransform(AttachedSocketName,RTS_World);
		FTransform targetTransform = OffsetTransform * socketTransform;
		
		FTransform worldTransform;
		worldTransform.Blend(TargetWorldTransform,targetTransform, alpha);
		
		SetWorldLocation(worldTransform.GetLocation());
		SetWorldRotation(worldTransform.GetRotation());
	}
}

void UEMAttachedBlendingComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
	BlendElipsedTime += deltaTime;
}