#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "Notify/EMAnimNotifyInterfaceMA.h"
#include "KMAnimNotifyState_MotionWarping.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Motion Warping")
class KMGAME_API UKMAnimNotifyState_MotionWarping : public UAnimNotifyState_MotionWarping, public IEMAnimNotifyInterfaceMA
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true))
	EEMNotifyGroupType GroupType = EEMNotifyGroupType::Animation;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

	virtual EEMNotifyGroupType GetGroupType() override { return GroupType; };
};
