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
	virtual void StartBlending(USceneComponent* newParentComponent, FName newAttachSocketName, const FTransform& startWorldTransform, float newDuration = 0.2f) override;
};