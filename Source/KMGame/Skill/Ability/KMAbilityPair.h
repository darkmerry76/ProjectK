#pragma once

#include "CoreMinimal.h"
#include "KMAbilitySkill.h"
#include "KMAbilityPair.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityPair : public UKMAbilitySkill
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activated() override;
	virtual void Deactivated(bool bCancel) override;

protected:
	virtual void PostActivated() override;
};