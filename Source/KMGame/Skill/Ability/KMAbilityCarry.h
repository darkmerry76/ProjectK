#pragma once

#include "CoreMinimal.h"
#include "KMAbilitySkill.h"
#include "KMAbilityCarry.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityCarry : public UKMAbilitySkill
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;
	
	bool StartCarried(FName attackSocket, float blendingDuration = 0.2f);
	void CompleteCarried();
};