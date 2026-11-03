#pragma once

#include "CoreMinimal.h"
#include "KMAbilityEffect.h"
#include "KMAbilityBlow.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityBlow : public UKMAbilityEffect
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activated() override;
	virtual void Deactivated(bool bCancel = false) override;

	virtual void Impact(const FTransform& newImpactTransform) override;
	virtual void Trigger(const FGameplayTag& eventTag) override;
};