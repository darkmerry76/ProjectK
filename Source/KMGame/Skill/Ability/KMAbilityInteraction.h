#pragma once

#include "CoreMinimal.h"
#include "KMAbilitySkill.h"
#include "KMAbilityInteraction.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityInteraction : public UKMAbilitySkill
{
	GENERATED_UCLASS_BODY()

public:
	bool StartInteraction(FName attackSocket, float blendingDuration = 0.2f);
	
protected:	
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

protected:
	virtual void OnRequestEnd_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartInteraction();

	virtual class UKMGameObjectInstance* GetTargetGameObjectInstance() const override;
	virtual class AActor* GetTargetActor() const override;
};