#pragma once

#include "CoreMinimal.h"
#include "KMAttachedBlendingComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class KMGAME_API UKMAttachedBlendingComponent : public USceneComponent
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