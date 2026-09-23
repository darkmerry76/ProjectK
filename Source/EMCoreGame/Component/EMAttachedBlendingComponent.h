#pragma once

#include "CoreMinimal.h"
#include "EMTransformUpdateInterface.h"
#include "EMAttachedBlendingComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Abstract)
class EMCOREGAME_API UEMAttachedBlendingComponent : public USceneComponent, public IEMTransformUpdateInterface
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform OffsetTransform;

	UPROPERTY(Transient)
	TWeakObjectPtr<class USceneComponent> AttachedParentComponent;
	
public:
	virtual void StartBlending(class USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& newTargetWorldTransform, float newDuration = 0.2f);
	virtual void StopBlending();

	virtual void UpdateTransform(float deltaTime) override;

	const FTransform& GetOffsetTransform() const;
	
protected:
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	
protected:
	FName AttachedSocketName;
	FTransform TargetWorldTransform;
	
	float Duration = 0.2f;
	float BlendElipsedTime = 0.f;
};