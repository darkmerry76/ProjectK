#include "KMAnimNode_PoseSnapShotBlend.h"
#include "Animation/AnimInstanceProxy.h"
#include "Character/KMChainAnimInstance.h"
#include "Animation/AttributesRuntime.h"

void FKMAnimNode_PoseSnapShotBlend::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	AttackPose.Initialize(context);
}

void FKMAnimNode_PoseSnapShotBlend::Update_AnyThread(const FAnimationUpdateContext& context)
{
	AttackPose.Update(context);

	Time += context.GetDeltaTime() * 20.f;
}

void FKMAnimNode_PoseSnapShotBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& context)
{
	AttackPose.CacheBones(context);

	const FBoneContainer& boneContainer = context.AnimInstanceProxy->GetRequiredBones();

	ChainRootBone.Initialize(boneContainer);
	ChainRootIndex = ChainRootBone.GetCompactPoseIndex(boneContainer);
}

void FKMAnimNode_PoseSnapShotBlend::Evaluate_AnyThread(FPoseContext& output)
{
	FPoseContext attackContext(output);
	AttackPose.Evaluate(attackContext);
	
	output = attackContext;

	const FBoneContainer& boneContainer = output.Pose.GetBoneContainer();
	const UKMChainAnimInstance* animInstance = Cast<UKMChainAnimInstance>(attackContext.GetAnimInstanceObject());

	FPoseSnapshot snapshot = animInstance->Snapshot;
	if (animInstance->GetWorld()->IsGameWorld() == true)
	{
		if (!ChainRootIndex.IsValid() || !animInstance->EnableAttack)
		{
			return;
		}
	}
	
    const float clampedAlpha = animInstance->EnableAttack ? animInstance->BlendAlpha : Alpha;

	int32 rootIndex = ChainRootIndex.GetInt();
    int32 boneCount = output.Pose.GetNumBones();

	float prevAngle = 0;
	int32 chainLength = boneCount - rootIndex;
    for (int32 i = rootIndex; i < boneCount; i++)
    {
        FCompactPoseBoneIndex boneIndex(i);
        if (!boneContainer.BoneIsChildOf(boneIndex, ChainRootIndex))
            continue;

    	FTransform boneTM = output.Pose[boneIndex];
    	if (boneIndex == ChainRootIndex)
    	{
    		boneTM.SetRotation(FQuat::Identity);
    		output.Pose[boneIndex] = boneTM;
    		continue;
    	}
    	
        FVector refLoc = boneTM.GetTranslation();
    	FVector newLoc = refLoc * clampedAlpha;

    	boneTM.SetTranslation(newLoc);

	   	const int32 chainIndex = i - rootIndex;

    	const float t = static_cast<float>(chainIndex) / static_cast<float>(chainLength);
    	const float envelope = FMath::Pow(FMath::Sin(t * PI), 1.5f);
    	const float finalAmplitude = WaveAmplitude * envelope * (1.0f - clampedAlpha);
    	const float phase = chainIndex * WaveFrequency + Time * WaveSpeed - chainIndex;
    	const float angle = FMath::Sin(phase) * finalAmplitude;

    	const float localAngle = angle - prevAngle;
    	prevAngle = angle;

    	const FQuat WaveRot(FVector::UpVector,FMath::DegreesToRadians(localAngle));
    	const FQuat WaveRot2(FVector::RightVector,FMath::DegreesToRadians(localAngle));

    	boneTM.SetRotation(WaveRot);
    	output.Pose[boneIndex] = boneTM;
    }
}