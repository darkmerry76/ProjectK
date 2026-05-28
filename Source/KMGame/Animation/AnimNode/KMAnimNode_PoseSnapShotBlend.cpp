#include "KMAnimNode_PoseSnapShotBlend.h"
#include "Animation/AnimInstanceProxy.h"
#include "Character/KMChainAnimInstance.h"
#include "Animation/AttributesRuntime.h"

void FKMAnimNode_PoseSnapShotBlend::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	AttackPose.Initialize(Context);
}

void FKMAnimNode_PoseSnapShotBlend::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	AttackPose.Update(Context);

	Time += Context.GetDeltaTime() * 20.f;
}

void FKMAnimNode_PoseSnapShotBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	AttackPose.CacheBones(Context);

	const FBoneContainer& BoneContainer =
		Context.AnimInstanceProxy->GetRequiredBones();

	ChainRootBone.Initialize(BoneContainer);

	ChainRootIndex =
		ChainRootBone.GetCompactPoseIndex(BoneContainer);
}

void FKMAnimNode_PoseSnapShotBlend::Evaluate_AnyThread(FPoseContext& Output)
{
	FPoseContext AttackContext(Output);
	AttackPose.Evaluate(AttackContext);
	Output = AttackContext;

	const FBoneContainer& BoneContainer = Output.Pose.GetBoneContainer();
	
	const UKMChainAnimInstance* animInstance = Cast<UKMChainAnimInstance>(AttackContext.GetAnimInstanceObject());

	FPoseSnapshot Snapshot = animInstance->Snapshot;

	if (animInstance->GetWorld()->IsGameWorld() == true)
	{
		if (Snapshot.bIsValid == false)
		{
			Output = AttackContext;
			return;
		}

		if (!ChainRootIndex.IsValid() || !animInstance->EnableAttack)
		{
			return;
		}
	}
	
    const float ClampedAlpha = FMath::Clamp(animInstance->GetWorld()->IsGameWorld() == true ? animInstance->BlendAlpha : Alpha, 0.f, 1.f);

    int32 RootIndex = ChainRootIndex.GetInt();
    int32 BoneCount = Output.Pose.GetNumBones();

	float PrevAngle = 0;
	int32 ChainLength = BoneCount - RootIndex;
    for (int32 i = RootIndex; i < BoneCount; i++)
    {
        FCompactPoseBoneIndex BoneIndex(i);

        if (!BoneContainer.BoneIsChildOf(BoneIndex, ChainRootIndex))
            continue;

    	FTransform BoneTM = Output.Pose[BoneIndex];
    	if (BoneIndex == ChainRootIndex)
    	{
    		// 위치는 따라가되 rotation은 고정
    		BoneTM.SetRotation(FQuat::Identity);

    		Output.Pose[BoneIndex] = BoneTM;
    		continue;
    	}

        

        FVector RefLoc = BoneTM.GetTranslation();
    	FVector NewLoc = RefLoc * ClampedAlpha;

    	BoneTM.SetTranslation(NewLoc);

	   	const int32 ChainIndex = i - RootIndex;

    	const float t = (float)ChainIndex / (float)ChainLength;
    	const float Envelope = FMath::Pow(FMath::Sin(t * PI), 1.5f);
    	const float FinalAmplitude =
			WaveAmplitude * Envelope * (1.0f - ClampedAlpha);

    	const float Phase =
			ChainIndex * WaveFrequency +
			Time * WaveSpeed -
			ChainIndex;

    	const float Angle =
			FMath::Sin(Phase) * FinalAmplitude;

    	const float LocalAngle = Angle - PrevAngle;
    	PrevAngle = Angle;

    	const FQuat WaveRot(
			FVector::UpVector,
			FMath::DegreesToRadians(LocalAngle)
		);
    	const FQuat WaveRot2(FVector::RightVector,
	FMath::DegreesToRadians(LocalAngle)
		);

    	BoneTM.SetRotation(WaveRot);

    	Output.Pose[BoneIndex] = BoneTM;
    }

/*	if (animInstance->GetWorld()->IsGameWorld() == true)
	{
		FCompactPose SnapshotPose;
		FBlendedCurve SnapshotCurve;

		SnapshotPose.SetBoneContainer(&BoneContainer);
		SnapshotPose.ResetToRefPose();

		for (int32 i = 0; i < Snapshot.BoneNames.Num(); ++i)
		{
			const FName BoneName = Snapshot.BoneNames[i];

			const int32 MeshBoneIndex =
				BoneContainer.GetReferenceSkeleton().FindBoneIndex(BoneName);

			if (MeshBoneIndex == INDEX_NONE)
				continue;

			FCompactPoseBoneIndex CompactIndex =
				BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshBoneIndex));

			if (CompactIndex != INDEX_NONE)
			{
				SnapshotPose[CompactIndex] =
					Snapshot.LocalTransforms[i];
			}
		}

		float snapShotBlendAlpha = FMath::Clamp(ClampedAlpha * 5.f, 0.f, 1.f);
		
		for (int32 i = RootIndex; i < BoneCount; ++i)
		{
			FCompactPoseBoneIndex BoneIndex(i);

			if (!BoneContainer.BoneIsChildOf(BoneIndex, ChainRootIndex))
				continue;

			FTransform& PoseTM = Output.Pose[BoneIndex];
			const FTransform& SnapTM = SnapshotPose[BoneIndex];

			PoseTM.SetTranslation(
				FMath::Lerp(
				SnapTM.GetTranslation(),
					PoseTM.GetTranslation(),
					snapShotBlendAlpha
				)
			);

			PoseTM.SetRotation(
				FQuat::Slerp(
				SnapTM.GetRotation(),
					PoseTM.GetRotation(),
					snapShotBlendAlpha
				)
			);
		}
	}*/
}