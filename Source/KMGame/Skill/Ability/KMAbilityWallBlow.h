#pragma once

#include "CoreMinimal.h"
#include "KMAbilityBlow.h"
#include "KMAbilityWallBlow.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityWallBlow : public UKMAbilityBlow
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

	virtual void Impact(const FTransform& newImpactTransform) override;
	virtual void OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode movementMode) override;
};