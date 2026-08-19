#pragma once

#include "CoreMinimal.h"
#include "KMAbility.h"
#include "KMAbilitySkill.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySkill : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EndingTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDirectionFallow = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsDirectionFallow"))
	bool bIsForceRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsDirectionFallow"))
	float DirectionWeight = 1.f;

protected:
	TWeakPtr<class FKMSkillInstance> SkillInstance;

public:
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;

	virtual void PostActivated();

	void SetSkillInstance(const TSharedPtr<class FKMSkillInstance>& newSkillInstance);
	TWeakPtr<class FKMSkillInstance> GetSkillInstance() const;

protected:
	virtual void ForceComplate() override;
};