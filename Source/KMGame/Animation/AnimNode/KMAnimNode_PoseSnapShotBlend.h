#pragma once

#include "CoreMinimal.h"
#include "KMAnimNode_PoseSnapShotBlend.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct KMGAME_API FKMAnimNode_PoseSnapShotBlend : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Snapshot", meta=(PinShownByDefault))
	//float Alpha = 0.f;

	UPROPERTY(EditAnywhere, Category="Chain")
	FBoneReference ChainRootBone;

	FCompactPoseBoneIndex ChainRootIndex = FCompactPoseBoneIndex(INDEX_NONE);;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Links")
	FPoseLink AttackPose;

	UPROPERTY(EditAnywhere, Category="Chain")
	float Alpha = 0.f;

	float Time = 0.f;

	float BoneLength = 50.f;

	UPROPERTY(EditAnywhere, Category="Chain")
	float WaveAmplitude = 80.f;

	UPROPERTY(EditAnywhere, Category="Chain")
	float WaveSpeed = 8.f;

	UPROPERTY(EditAnywhere, Category="Chain")
	float WaveFrequency = 100.f;

	UPROPERTY(EditAnywhere, Category="Chain")
	float WavePropagation = 0.2f;

	UPROPERTY(EditAnywhere, Category="Chain")
	float DeltaTime = 1.f / 60.f;

public:
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
};