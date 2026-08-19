#pragma once

#include "CoreMinimal.h"
#include "KMAbilityEffect.h"
#include "KMAbilityPaired.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityPaired : public UKMAbilityEffect
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D PairOffset = FVector2D::ZeroVector;

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

protected:
	virtual void PostActivated() override;
};