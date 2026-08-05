#pragma once

#include "CoreMinimal.h"
#include "KMAbilityEffect.h"
#include "KMAbilityBlow.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityBlow : public UKMAbilityEffect
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel = false) override;

	virtual void Impact(const FTransform& newImpactTransform) override;
	virtual void OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType type) override;
};