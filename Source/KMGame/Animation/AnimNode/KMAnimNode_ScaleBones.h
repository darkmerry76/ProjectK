#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "KMAnimNode_ScaleBones.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct KMGAME_API FKMAnimNode_ScaleBones : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Bone)
	FVector Scale = FVector(1.0f);

	UPROPERTY()
	TArray<FBoneReference> Bones;

private:
	TArray<FCompactPoseBoneIndex> CompactBoneIndices;

	UPROPERTY()
	FPoseLink BasePose;

	TArray<FCompactPoseBoneIndex> BoneIndices;

protected:
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& context) override;
	virtual bool IsValidToEvaluate(
		const USkeleton* Skeleton,
		const FBoneContainer& RequiredBones
	) override
	{
		return Bones.Num() > 0;
	}
	
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& output,
		TArray<FBoneTransform>& outBoneTransforms) override;
	
	
};