#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "Notify/EMAnimNotifyStateMA.h"
#include "KMAnimNotifyState_Animation.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Animation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, const FAnimNotifyEventReference& eventReference) override;
};
