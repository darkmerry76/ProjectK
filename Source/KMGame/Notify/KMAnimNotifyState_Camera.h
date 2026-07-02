#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "Core/KMParameterValue.h"
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

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="CameraSequence"))
	FKMParameterSimpleScalar BlendIn;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendIn"))
	float BlendInTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendIn"))
    FKMParameterSimpleScalar BlendOut;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendIn"))
	bool bIsImmadiate = false;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(EditCondition="!bIsImmadiate", AllowPrivateAccess=true, DisplayAfter="BlendOut"))
	float BlendOutTime = 0.2f;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendIn"))
	float FadeInTime = 0.2f;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendIn"))
	float FadeOutTime = 0.2f;
    	
	UPROPERTY(Transient)
	TMap<class USkeletalMeshComponent*, float> AnimationTimes;

	UPROPERTY()
	TWeakObjectPtr<class UKMCameralayerOverlaySequence> CameraOverlayLayer;

protected:
	TSharedPtr<struct FEMCameraCacheInstance> CameraCacheInstance;
	TWeakPtr<class FEMCameraCacheManager> CameraCacheManager;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
