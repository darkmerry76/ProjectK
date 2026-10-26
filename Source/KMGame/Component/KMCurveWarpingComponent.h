#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingComponent.h"
#include "KMCurveWarpingComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="CurveWarpingComponent", meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMCurveWarpingComponent : public UEMCurveWarpingComponent
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void SetOwnerMovementMode(EMovementMode newMovementMode) override;
	
public:
	virtual int32 PlayLinearWarp(FVector targetWarpLocation, float newDuration) override;
};