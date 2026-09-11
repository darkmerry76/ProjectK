#include "KMAnimNode_PoseSnapShotBlend.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AttributesRuntime.h"
#include "GameActor/Pawn/Character/KMChainAnimInstance.h"

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

	FKMChainAnimInstanceProxy* chainAnimInstanceProxy = static_cast<FKMChainAnimInstanceProxy*>(attackContext.AnimInstanceProxy);
	check(chainAnimInstanceProxy);

	if (output.GetAnimInstanceObject()->GetWorld()->IsGameWorld() == true)
	{
		if (!ChainRootIndex.IsValid() || !chainAnimInstanceProxy->IsEnableAttack())
		{
			return;
		}
	}
	
    const float clampedAlpha = chainAnimInstanceProxy->IsEnableAttack() ? chainAnimInstanceProxy->GetBlendAlpha() : Alpha;

	int32 rootIndex = ChainRootIndex.GetInt();
    int32 boneCount = output.Pose.GetNumBones();

	float prevAngleX = 0.f;
	float prevAngleZ = 0.f;
	int32 chainLength = boneCount - rootIndex;
    for (int32 i = rootIndex; i < boneCount; i++)
    {
        FCompactPoseBoneIndex boneIndex(i);
    	
    	if (boneIndex == ChainRootIndex)
    	{
    		output.Pose[boneIndex] = output.Pose[boneIndex] *
    			FTransform(CalcChainTargetRotation(output, chainAnimInstanceProxy->GetTargetLocation()));
    		continue;
    	}

    	FTransform boneTM = output.Pose[boneIndex];
        FVector refLoc = boneTM.GetTranslation();
    	FVector newLoc = refLoc * clampedAlpha;

    	boneTM.SetTranslation(newLoc);

	   	const int32 chainIndex = i - rootIndex;

    	const float t = static_cast<float>(chainIndex) / static_cast<float>(chainLength);
    	const float envelope = FMath::Pow(FMath::Sin(t * PI), 1.5f);
    	const float finalAmplitude = WaveAmplitude * envelope * (1.0f - clampedAlpha);
    	const float phase = chainIndex * WaveFrequency + Time * WaveSpeed - chainIndex;
    	
    	const float angleX = FMath::Sin(phase) * finalAmplitude;
    	const float angleZ = FMath::Sin(phase) * finalAmplitude;

    	const float localAngleX = angleX - prevAngleX;
    	const float localAngleZ = angleZ - prevAngleZ;

    	prevAngleX = angleX;
    	prevAngleZ = angleZ;

    	const FQuat waveRotX(FVector::ForwardVector,FMath::DegreesToRadians(localAngleX));

    	const FQuat waveRotZ(FVector::UpVector, FMath::DegreesToRadians(localAngleZ));

    	boneTM.SetRotation(waveRotZ);
    	output.Pose[boneIndex] = boneTM;
    }
}

FQuat FKMAnimNode_PoseSnapShotBlend::CalcChainTargetRotation(const FPoseContext& output, const FVector& targetLocation) const
{
	if (!ChainRootIndex.IsValid())
	{
		return FQuat::Identity;
	}

	FCSPose<FCompactPose> componentPose;
	componentPose.InitPose(output.Pose);

	const FTransform chainRootComponentTM = componentPose.GetComponentSpaceTransform(ChainRootIndex);
	const FAnimInstanceProxy* animInstanceProxy = output.AnimInstanceProxy;
	if (!animInstanceProxy)
	{
		return FQuat::Identity;
	}

	const FTransform componentTransform = animInstanceProxy->GetComponentTransform();
	const FVector chainRootWorldLocation = componentTransform.TransformPosition(chainRootComponentTM.GetLocation());
	FVector targetDirectionWorld = targetLocation - chainRootWorldLocation;
	if (!targetDirectionWorld.Normalize())
	{
		return output.Pose[ChainRootIndex].GetRotation();
	}

	const FVector targetDirectionComponent = componentTransform.InverseTransformVectorNoScale(targetDirectionWorld).GetSafeNormal();
	return FQuat::FindBetweenNormals(FVector::ForwardVector, targetDirectionComponent);
}