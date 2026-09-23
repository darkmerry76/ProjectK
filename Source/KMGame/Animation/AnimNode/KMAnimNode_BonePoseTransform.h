#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimTypes.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/BoneReference.h"
#include "KMAnimNode_BonePoseTransform.generated.h"

struct KMGAME_API FKMAnimNodeShakeData
{
	bool bIsEnable = false;
	
	float Duration = 0.2f;
	float ElapsedTime = 0.f;

	float Frequency = 10.f;
	float Distance = 10.f;

	FVector CameraUp;
	FVector CameraRight;

	bool IsValid() const
	{
		return bIsEnable;
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct KMGAME_API FKMAnimNode_BonePoseTransform : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category=Links)
	FPoseLink Source;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	bool bUsedShake = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings, meta=(EditCondition=bUsedShake))
	TObjectPtr<class UCurveVector> ShakeCurve;
	
	UPROPERTY(EditAnywhere, Category=Settings, meta=(EditCondition=bUsedShake))
	FBoneReference ShakeBone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	bool bUsedPairBone = false;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(EditCondition=bUsedPairBone))
	FBoneReference PairRootBone;

	FCompactPoseBoneIndex ShakeBoneIndex;
	FCompactPoseBoneIndex PairRootBoneIndex;

public:	
	FKMAnimNode_BonePoseTransform();

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& context) override;
	virtual void Evaluate_AnyThread(FPoseContext& output) override;
	virtual void GatherDebugData(FNodeDebugData& debugData) override;
};
