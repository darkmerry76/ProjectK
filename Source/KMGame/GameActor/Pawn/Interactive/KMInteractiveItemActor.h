#pragma once

#include "CoreMinimal.h"
#include "KMInteractiveActorBase.h"
#include "KMInteractiveItemActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMInteractiveItemActor : public AKMInteractiveActorBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) override;
	virtual void OnDeath() override;
	
	virtual FBoxSphereBounds GetMasterBounds() const override;
};