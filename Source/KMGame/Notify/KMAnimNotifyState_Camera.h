#pragma once

#include "CoreMinimal.h"
#include "Notify/EMAnimNotifyStateMA.h"
#include "KMAnimNotifyState_Camera.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Camera
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="Play Camera")
class KMGAME_API UKMAnimNotifyState_Camera : public UEMAnimNotifyStateMA
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify)
	TObjectPtr<class UCameraAnimationSequence> CameraSequence;

	UPROPERTY(Transient)
	TMap<class USkeletalMeshComponent*, float> AnimationTimes;

protected:
	TSharedPtr<struct FEMCameraCache> CameraCache;
	TWeakPtr<class FEMCameraCacheManager> CameraCacheManager;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
};
