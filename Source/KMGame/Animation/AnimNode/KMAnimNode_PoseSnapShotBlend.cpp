#include "KMAnimNode_PoseSnapShotBlend.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AttributesRuntime.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "GameActor/Pawn/Character/KMChainAnimInstance.h"

void FKMAnimNode_PoseSnapShotBlend::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	AttackPose.Initialize(context);
}

void FKMAnimNode_PoseSnapShotBlend::Update_AnyThread(const FAnimationUpdateContext& context)
{
	AttackPose.Update(context);

	FKMChainAnimInstanceProxy* chainAnimInstanceProxy = static_cast<FKMChainAnimInstanceProxy*>(context.AnimInstanceProxy);
	DeltaSeconds = context.GetDeltaTime() * chainAnimInstanceProxy->GetCustomTimeDilation(); 
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


	float deltaSeconds = DeltaSeconds;

	Time += deltaSeconds * 20.f;

	const FTransform componentTransform = chainAnimInstanceProxy->GetComponentTransform();

	FCSPose<FCompactPose> componentPose;
	componentPose.InitPose(output.Pose);
	
	const FTransform chainRootComponentTM = componentPose.GetComponentSpaceTransform(ChainRootIndex);
	const FVector chainRootWorldLocation = componentTransform.TransformPosition(chainRootComponentTM.GetLocation());

	const float targetDistance = output.GetAnimInstanceObject()->GetWorld()->IsGameWorld() ? FVector::Distance(chainRootWorldLocation, chainAnimInstanceProxy->GetTargetLocation()) : 525.f;

	FQuat targetQut = output.GetAnimInstanceObject()->GetWorld()->IsGameWorld() ? CalcChainTargetRotation(output, chainAnimInstanceProxy->GetTargetLocation()) : FQuat::Identity;

    const float clampedAlpha = chainAnimInstanceProxy->IsEnableAttack() ? chainAnimInstanceProxy->GetBlendAlpha() : Alpha;

	int32 rootIndex = ChainRootIndex.GetInt();
    int32 boneCount = output.Pose.GetNumBones();

	float prevAngleX = 0.f;
	float prevAngleZ = 0.f;
	int32 chainLength = boneCount - rootIndex;

	float chainLengthScale = FMath::Max(targetDistance / 525.f, 1.f);

	float currentChainLength = 0.f;

	float speed = 0.f;
	if (deltaSeconds > 0.0001f)
	{
		const FVector rootVelocity = (chainRootWorldLocation - PreviousChainRootLocation) / (deltaSeconds > 0.0001f ? deltaSeconds : 1.f);
		const FVector targetVelocity = (chainAnimInstanceProxy->GetTargetLocation() - PreviousTargetLocation) / deltaSeconds;
		speed = FMath::Max(rootVelocity.Size() * 0.5f, targetVelocity.Size());
	}
	
	constexpr float waveStartSpeed = 350.f;
	constexpr float waveMaxSpeed = 750.f;

	const float targetSpeedAlpha = FMath::GetMappedRangeValueClamped(FVector2D(waveStartSpeed, waveMaxSpeed),FVector2D(0.f, 1.f), speed);
	WaveAlpha = FMath::FInterpTo(WaveAlpha, targetSpeedAlpha, deltaSeconds, WaveInterpSpeed);

	FTransform parentWorldTM = FTransform::Identity;

    for (int32 i = rootIndex; i < boneCount - 2; i++)
    {
        FCompactPoseBoneIndex boneIndex(i);
    	
    	if (boneIndex == ChainRootIndex)
    	{
    		output.Pose[boneIndex] = output.Pose[boneIndex] * FTransform(targetQut);
    		continue;
    	}
    	
    	FTransform boneTM = output.Pose[boneIndex];

    	float previusChainLength = currentChainLength;
    	currentChainLength += output.Pose[boneIndex].GetTranslation().Size();
    	if (currentChainLength < targetDistance)
    	{
    		FVector refLoc = boneTM.GetTranslation();
    		FVector newLoc = refLoc * clampedAlpha;

    		boneTM.SetTranslation(newLoc);

    		const int32 chainIndex = i - rootIndex;

    		const float t = static_cast<float>(chainIndex) / static_cast<float>(chainLength);

    		const float envelope = FMath::Pow(FMath::Sin(t * PI), 1.5f);
    	
    		float finalAmplitude = 0.f;
    		float finalWaveFrequency = 0.f;

    		if (clampedAlpha < 1.f)
    		{
    			finalAmplitude = WaveAmplitude * envelope * (1.f - clampedAlpha);
    			finalWaveFrequency = WaveFrequency;
    		}
    		else
    		{
    			finalAmplitude = FMath::Lerp(WaveAmplitude, WaveAmplitude2, WaveAlpha) * envelope * (1.01f - clampedAlpha);

    			const float distanceScale = FMath::Clamp(targetDistance / 1000.f, 0.f, 1.f);
    			finalWaveFrequency = FMath::Lerp(WaveFrequency2, WaveFrequency, distanceScale);
    		}

    		const float phase = chainIndex * finalWaveFrequency + Time * WaveSpeed - chainIndex;
    		
    		const float angleX = FMath::Sin(phase * 0.5f) * finalAmplitude;
    		const float angleZ = FMath::Sin(phase) * finalAmplitude;

    		const float localAngleX = angleX - prevAngleX;
    		const float localAngleZ = angleZ - prevAngleZ;

    		prevAngleX = angleX;
    		prevAngleZ = angleZ;

    		const FQuat waveRotX(FVector::ForwardVector,FMath::DegreesToRadians(localAngleX));

    		const FQuat waveRotZ(FVector::UpVector, FMath::DegreesToRadians(localAngleZ));

    		boneTM.SetRotation(waveRotZ);
    	}
    	
    	if (previusChainLength < targetDistance && currentChainLength >= targetDistance)
    	{
    		boneTM.SetLocation(FVector(0.f, previusChainLength - targetDistance, 0.f));
    	}
    	else if (currentChainLength >= targetDistance)
    	{
    		boneTM.SetLocation(FVector::ZeroVector);
    	}

    	boneTM.SetLocation(boneTM.GetTranslation() * chainLengthScale);

    	output.Pose[boneIndex] = boneTM;
    }
	
	PreviousChainRootLocation = chainRootWorldLocation;
	PreviousTargetLocation = chainAnimInstanceProxy->GetTargetLocation();

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