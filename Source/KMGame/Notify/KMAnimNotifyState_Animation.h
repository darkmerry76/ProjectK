#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Notify/EMAnimNotifyStateMA.h"
#include "KMAnimNotifyState_Animation.generated.h"

USTRUCT(BlueprintType)
struct KMGAME_API FEMAnimationSetTag
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;
};

UCLASS(Blueprintable, BlueprintType, DisplayName="Play Animation")
class KMGAME_API UKMAnimNotifyState_Animation : public UEMAnimNotifyStateMA
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify)
	bool bUseSkillSet = true;
	
	UPROPERTY(VisibleAnywhere, Category=AnimNotify, BlueprintReadOnly, meta=(AllowPrivateAccess=true, EditCondition="bUseSkillSet", DisplayAfter=""))
	FEMAnimationSetTag AnimationSetTag;

	UPROPERTY(EditAnywhere, Category=AnimNotify)
	TObjectPtr<class UAnimMontage> Montage; 
};
