#pragma once

#include "CoreMinimal.h"
#include "KMAbilityEffect.h"
#include "KMAbilityBlow.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityBlow : public UKMAbilityEffect
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCurveVector> BlowCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HorizontalPower = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalPower = 100.f;
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;

	virtual void Impact(const FTransform& newImpactTransform) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnCurveWarpingInterrupt(const FVector& moveDelta, EEMCurveWarpingInteruptType type);
};