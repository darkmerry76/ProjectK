#pragma once

#include "CoreMinimal.h"
#include "Component/EMAttachedBlendingComponent.h"
#include "KMAttachedBlendingComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAttachedBlendingComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMAttachedBlendingComponent : public UEMAttachedBlendingComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void StartBlending(class USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& targetWorldTransform, float newDuration = 0.2f) override;
	virtual void StopBlending() override;
};