#pragma once

#include "CoreMinimal.h"
#include "EMAttachedBlendingComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Abstract)
class EMCOREGAME_API UEMAttachedBlendingComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	void StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, const FTransform& offsetTransform, float newDuration = 0.2f);
	void StopBlending();
	
protected:
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	void UpdateBlending(float deltaTime);

protected:
	FName AttachedSocketName;
	FTransform StartWorldTransform;
	FTransform OffsetTransform;
	float Duration = 0.2f;
	float BlendElipsedTime = 0.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<USceneComponent> AttachedParentComponent;
};