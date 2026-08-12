#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimTypes.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/BoneReference.h"
#include "KMAnimNode_Shake.generated.h"

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
struct KMGAME_API FKMAnimNode_Shake : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category=Links)
	FPoseLink Source;

	UPROPERTY(EditAnywhere, Category=Settings)
	TObjectPtr<class UCurveVector> ShakeCurve;

	UPROPERTY(EditAnywhere, Category=Settings)
	FBoneReference ShakeBone;

	FCompactPoseBoneIndex BoneIndex;

public:	
	FKMAnimNode_Shake();

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& context) override;
	virtual void Evaluate_AnyThread(FPoseContext& output) override;
	virtual void GatherDebugData(FNodeDebugData& debugData) override;
};
