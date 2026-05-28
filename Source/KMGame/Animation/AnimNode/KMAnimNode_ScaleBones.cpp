#include "KMAnimNode_ScaleBones.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/KMAnimInstance.h"

void FKMAnimNode_ScaleBones::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	Super::Initialize_AnyThread(context);
	
	BasePose.Initialize(context);

	const FBoneContainer& boneContainer = context.AnimInstanceProxy->GetRequiredBones();
	
	Bones.Empty();

	if (const UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(context.GetAnimInstanceObject()))
	{
		for (const FName& boneName : animInstance->GetHiddenBones())
		{
			FBoneReference boneRef;
			boneRef.BoneName = boneName;
			boneRef.Initialize(boneContainer);
			Bones.Emplace(boneRef);
		}
	}
}

void FKMAnimNode_ScaleBones::CacheBones_AnyThread(const FAnimationCacheBonesContext& context)
{
	Super::CacheBones_AnyThread(context);
	
	BasePose.CacheBones(context);

	const FBoneContainer& boneContainer = context.AnimInstanceProxy->GetRequiredBones();

	BoneIndices.Reset();

	for (FBoneReference& bone : Bones)
	{
		FCompactPoseBoneIndex index = bone.GetCompactPoseIndex(boneContainer);
		if (index != INDEX_NONE)
		{
			BoneIndices.Add(index);
		}
	}
}

void FKMAnimNode_ScaleBones::EvaluateSkeletalControl_AnyThread(
	FComponentSpacePoseContext& output,
	TArray<FBoneTransform>& outBoneTransforms)
{
	for (FCompactPoseBoneIndex boneIndex : BoneIndices)
	{
		FTransform boneTransform = output.Pose.GetComponentSpaceTransform(boneIndex);
		boneTransform.SetScale3D(Scale);
		outBoneTransforms.Add(
			FBoneTransform(boneIndex, boneTransform)
		);
	}

	outBoneTransforms.Sort(FCompareBoneTransformIndex());
}