#include "KMAnimNode_Shake.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimStats.h"
#include "Animation/KMAnimInstance.h"
#include "Curves/CurveVector.h"

FKMAnimNode_Shake::FKMAnimNode_Shake() : ShakeCurve(nullptr)
                                         , BoneIndex(INDEX_NONE)
{
}

void FKMAnimNode_Shake::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(context);
	Source.Initialize(context);
}

void FKMAnimNode_Shake::CacheBones_AnyThread(const FAnimationCacheBonesContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	Source.CacheBones(context);

	const FBoneContainer& boneContainer = context.AnimInstanceProxy->GetRequiredBones();
	ShakeBone.Initialize(boneContainer);
	BoneIndex = ShakeBone.GetCompactPoseIndex(boneContainer);
}

void FKMAnimNode_Shake::Update_AnyThread(const FAnimationUpdateContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
	Source.Update(context);
}

void FKMAnimNode_Shake::Evaluate_AnyThread(FPoseContext& output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(Slot, !IsInGameThread());

	Source.Evaluate(output);
	if (!BoneIndex.IsValid())
	{
		return;
	}
	if (!IsValid(ShakeCurve))
	{
		return;
	}

	const FKMAnimInstanceProxy& animInstanceProxy = static_cast<const FKMAnimInstanceProxy&>(*output.AnimInstanceProxy);
	const FKMAnimNodeShakeData& shakeData = animInstanceProxy.GetShakeData();
	if (!shakeData.IsValid())
	{
		return;
	}
	
	FTransform boneTM = output.Pose[BoneIndex];

	const float alpha = FMath::Clamp(shakeData.ElapsedTime / shakeData.Duration, 0.f, 1.f);

	float minTime = 0.f, maxTime = 1.f;
	ShakeCurve->GetTimeRange(minTime, maxTime);

	float finalTime = FMath::Lerp(minTime, maxTime, alpha);

	FVector curveVector = ShakeCurve->GetVectorValue(finalTime);
	
	const FVector shakeOffset  = (shakeData.CameraRight * curveVector.X + shakeData.CameraUp * curveVector.Z) * shakeData.Distance;

	const FTransform& componentTM = animInstanceProxy.GetComponentTransform();
	
	const FVector shakeOffsetCS = componentTM.InverseTransformVectorNoScale(shakeOffset);
	FVector addLocation = shakeOffsetCS;
	boneTM.AddToTranslation(addLocation);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("x=%.3f y=%.3f z=%.3f"), addLocation.X, addLocation.Y, addLocation.Z));
	
	output.Pose[BoneIndex] = boneTM;
}

void FKMAnimNode_Shake::GatherDebugData(FNodeDebugData& debugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	
	FString debugLine = debugData.GetNodeName(this);
}