#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Camera.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Camera
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Camera")
class KMGAME_API UKMAnimNotifyState_Camera : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
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

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
