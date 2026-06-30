#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "Core/KMParameterValue.h"
#include "KMAnimNotifyState_FadeInout.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_FadeInout
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] FadeInout")
class KMGAME_API UKMAnimNotifyState_FadeInout : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	FKMParameterSimpleScalar FadeIn;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="FadeIn"))
	float FadeInTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="FadeInTime"))
    FKMParameterSimpleScalar FadeOut;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="FadeOut"))
	float FadeOutTime = 0.2f;

	UPROPERTY(Transient)
	TMap<class USkeletalMeshComponent*, float> AnimationTimes;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
	void SetFadeValue(class USkeletalMeshComponent* meshComp, float elipsedTime, const FAnimNotifyEventReference& eventReference);
};
