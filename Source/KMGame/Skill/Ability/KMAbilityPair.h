#pragma once

#include "CoreMinimal.h"
#include "KMAbilitySkill.h"
#include "KMAbilityPair.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityPair : public UKMAbilitySkill
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

protected:
	virtual void PostActivated() override;
};