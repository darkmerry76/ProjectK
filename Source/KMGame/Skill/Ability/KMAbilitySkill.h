#pragma once

#include "CoreMinimal.h"
#include "KMAbility.h"
#include "KMAbilitySkill.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySkill : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDirectionFallow = true;

protected:
	TWeakPtr<class FKMSkillInstance> SkillInstance;

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

	void SetSkillInstance(const TSharedPtr<class FKMSkillInstance>& newSkillInstance);
	TWeakPtr<class FKMSkillInstance> GetSkillInstance() const;

protected:
	virtual void Trigger(const FGameplayTag eventTag) override;
	virtual void ForceComplate() override;
};