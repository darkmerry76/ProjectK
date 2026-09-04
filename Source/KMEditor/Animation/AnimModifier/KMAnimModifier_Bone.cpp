#include "KMAnimModifier_Bone.h"

void UKMAnimModifier_Bone::OnApply_Implementation(UAnimSequence* animSequence)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	IAnimationDataController& controller = animSequence->GetController();
	
	const float duration = animSequence->GetPlayLength();
	const int32 numKeys = animSequence->GetNumberOfSampledKeys();

	TArray<FVector> posKeys;
	TArray<FQuat> rotKeys;
	TArray<FVector> scaleKeys;

	int32 boneIndex = animSequence->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}

	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);
	for (int32 i = 0; i < numKeys; ++i)
	{
		double time = animSequence->GetTimeAtFrame(i);

		FTransform outTransform;
		FAnimExtractContext extractContext(time, animSequence->bEnableRootMotion);
		animSequence->GetBoneTransform(outTransform, poseBoneIndex, extractContext, false);

		FVector currentPos = outTransform.GetLocation();
		FRotator currentRot = outTransform.GetRotation().Rotator();
		FVector currentScale = outTransform.GetScale3D();

		float curveVal = animSequence->EvaluateCurveData(TargetCurveName, time);

		double* resultValue = nullptr;

		switch (Type)
		{
		case EKM_BoneTransformType::TRANS_X: resultValue = &currentPos.X;break;
		case EKM_BoneTransformType::TRANS_Y: resultValue = &currentPos.Y;break;
		case EKM_BoneTransformType::TRANS_Z: resultValue = &currentPos.Z;break;
		case EKM_BoneTransformType::ROTATION_X: resultValue = &currentRot.Roll;break;
		case EKM_BoneTransformType::ROTATION_Y: resultValue = &currentRot.Pitch;break;
		case EKM_BoneTransformType::ROTATION_Z: resultValue = &currentRot.Yaw;break;
		case EKM_BoneTransformType::SCALE_X: resultValue = &currentScale.X;break;
		case EKM_BoneTransformType::SCALE_Y: resultValue = &currentScale.Y;break;
		case EKM_BoneTransformType::SCALE_Z: resultValue = &currentScale.Z;break;
		default:break;
		}

		if (resultValue)
		{
			switch (OperatorType)
			{
			case EKM_OperatorType::Set:(*resultValue) = curveVal; break;
			case EKM_OperatorType::Add:(*resultValue) += curveVal; break;
			case EKM_OperatorType::Subtract:(*resultValue) -= curveVal; break;
			case EKM_OperatorType::Multiply:(*resultValue) *= curveVal; break;
			case EKM_OperatorType::Divide:(*resultValue) /= curveVal; break;
			default:break;
			}
		}

		posKeys.Add(currentPos);
		rotKeys.Add(currentRot.Quaternion());
		scaleKeys.Add(currentScale);
	}
	
	controller.SetBoneTrackKeys(BoneName, posKeys, rotKeys, scaleKeys);
}

void UKMAnimModifier_BoneToRoot::OnApply_Implementation(UAnimSequence* animSequence)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	IAnimationDataController& controller = animSequence->GetController();
	
	const float duration = animSequence->GetPlayLength();
	const int32 numKeys = animSequence->GetNumberOfSampledKeys();

	TArray<FVector> bonePosKeys;
	TArray<FQuat> boneRotKeys;
	TArray<FVector> boneScaleKeys;

	TArray<FVector> rootPosKeys;
	TArray<FQuat> rootRotKeys;
	TArray<FVector> rootScaleKeys;
	
	int32 rootboneIndex = 0;
	int32 boneIndex = animSequence->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}

	FSkeletonPoseBoneIndex poseRootIndex(0);
	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);

	for (int32 i = 0; i < numKeys; ++i)
	{
		double time = animSequence->GetTimeAtFrame(i);

		FTransform outBoneTransform;
		FTransform outBaseTransform;
		FTransform outRootTransform;
		FAnimExtractContext extractBaseContext(static_cast<double>(0.f), animSequence->bEnableRootMotion);
		animSequence->GetBoneTransform(outBaseTransform, poseBoneIndex, extractBaseContext, false);

		FAnimExtractContext extractContext(time, animSequence->bEnableRootMotion);
		animSequence->GetBoneTransform(outBoneTransform, poseBoneIndex, extractContext, false);
		animSequence->GetBoneTransform(outRootTransform, poseRootIndex, extractContext, false);

		FVector offset = outBoneTransform.GetLocation() - outBaseTransform.GetLocation();
		FVector currentRootPos = FVector(CopyTranslateX ? offset.X : outRootTransform.GetLocation().X,
		CopyTranslateY ? offset.Y : outRootTransform.GetLocation().Y,
		CopyTranslateZ ? offset.Z : outRootTransform.GetLocation().Z);
		FRotator currentRootRot = outRootTransform.GetRotation().Rotator();
		FVector currentRootScale = outRootTransform.GetScale3D();
		
		rootRotKeys.Add(currentRootRot.Quaternion());
		rootScaleKeys.Add(currentRootScale);

		float xScale = X > 0 ? 1.f : -1.f;
		float yScale = Y > 0 ? 1.f : -1.f;
		float zScale = Z > 0 ? 1.f : -1.f;
		rootPosKeys.Add(FVector((&currentRootPos.X)[abs(X)] * xScale, (&currentRootPos.X)[abs(Y)] * yScale, (&currentRootPos.X)[abs(Z)] * zScale));
		
		FVector currentBonePos = FVector(
		DefaultTranslateX ? DefaultTranslate.X : outBoneTransform.GetLocation().X,
		DefaultTranslateY ? DefaultTranslate.Y : outBoneTransform.GetLocation().Y,
		DefaultTranslateZ ? DefaultTranslate.Z : outBoneTransform.GetLocation().Z);
		FRotator currentBoneRot = outBoneTransform.GetRotation().Rotator();
		FVector currentBoneScale = outBoneTransform.GetScale3D();

		bonePosKeys.Add(currentBonePos);
		boneRotKeys.Add(currentBoneRot.Quaternion());
		boneScaleKeys.Add(currentBoneScale);
	}
	
	controller.SetBoneTrackKeys(BoneName, bonePosKeys, boneRotKeys, boneScaleKeys);
	controller.SetBoneTrackKeys(TEXT("Root"), rootPosKeys, rootRotKeys, rootScaleKeys);
}

void UKMAnimModifier_BoneTransform::OnApply_Implementation(UAnimSequence* animSequence)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	IAnimationDataController& controller = animSequence->GetController();
	
	const int32 numKeys = animSequence->GetNumberOfSampledKeys();

	TArray<FVector> bonePosKeys;
	TArray<FQuat> boneRotKeys;
	TArray<FVector> boneScaleKeys;

	TArray<FVector> rootPosKeys;
	TArray<FQuat> rootRotKeys;
	TArray<FVector> rootScaleKeys;
	
	int32 boneIndex = animSequence->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}
	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);

	for (int32 i = 0; i < numKeys; ++i)
	{
		double Time = animSequence->GetTimeAtFrame(i);

		FTransform outBoneTransform;
		FAnimExtractContext extractContext(Time, animSequence->bEnableRootMotion);
		animSequence->GetBoneTransform(outBoneTransform, poseBoneIndex, extractContext, false);

		FQuat addQuat = AddRotator.Quaternion();
		FQuat currentBoneRot = addQuat * outBoneTransform.GetRotation();

		FVector currentBoneLocation = addQuat * ((outBoneTransform.GetLocation() + AddLocation) * MultiplyLocation);
		FVector currentBoneScale = (outBoneTransform.GetScale3D() + AddScale) * MultiplyScale;
		
		bonePosKeys.Add(currentBoneLocation);
		boneRotKeys.Add(currentBoneRot);
		boneScaleKeys.Add(currentBoneScale);
	}
	
	controller.SetBoneTrackKeys(BoneName, bonePosKeys, boneRotKeys, boneScaleKeys);
}

void UKMAnimModifier_FrameCut::OnApply_Implementation(UAnimSequence* animationSequence)
{
}

void UKMFixPelvisYawModifier::CreateBoneInfo(const UAnimSequence* animSequence, TArray<FKMBoneInfo>& outBoneInfos)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	const FReferenceSkeleton& refSkeleton = animSequence->GetSkeleton()->GetReferenceSkeleton();

	const int32 numBones = refSkeleton.GetNum();
	outBoneInfos.SetNum(numBones);
	for (int32 boneIndex = 0; boneIndex < numBones; ++boneIndex)
	{
		int32 parentBoneIndex = refSkeleton.GetParentIndex(boneIndex);
		if (parentBoneIndex != INDEX_NONE)
		{
			outBoneInfos[parentBoneIndex].ChildBoneIndices.Emplace(boneIndex);
		}
		outBoneInfos[boneIndex].ParentBoneIndex = parentBoneIndex;
		outBoneInfos[boneIndex].BoneName = refSkeleton.GetBoneName(boneIndex);
	}
}

void UKMFixPelvisYawModifier::FixedWorldPose(UAnimSequence* animSequence, int32 boneIndex, float time, const FTransform& parentTransform, TArray<FTransform>& outBoneWorldTransforms)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	for (auto childBoneIndex : BoneInfos[boneIndex].ChildBoneIndices)
	{
		FAnimExtractContext extractBaseContext(static_cast<double>(time), animSequence->bEnableRootMotion);

		FTransform boneLocalTransform;
		animSequence->GetBoneTransform(boneLocalTransform, FSkeletonPoseBoneIndex(childBoneIndex), extractBaseContext, false);
		outBoneWorldTransforms[childBoneIndex] = boneLocalTransform * parentTransform;

		FixedWorldPose(animSequence, childBoneIndex, time, outBoneWorldTransforms[childBoneIndex], outBoneWorldTransforms);
	}
}

void UKMFixPelvisYawModifier::OnApply_Implementation(UAnimSequence* animSequence)
{
	if (!IsValid(animSequence))
	{
		return;
	}
	
	CreateBoneInfo(animSequence, BoneInfos);
	
    IAnimationDataController& controller = animSequence->GetController();
    const FReferenceSkeleton& refSkeleton = animSequence->GetSkeleton()->GetReferenceSkeleton();
	
    FTransform offsetTransform(OffsetRotator);
	
    controller.OpenBracket(FText::FromString(TEXT("Perfect Matrix Inverse Rotation")));

	const int32 numKeys = animSequence->GetNumberOfSampledKeys();

	TArray<TArray<FVector>> newLocation;
	TArray<TArray<FQuat>> newRotation;
	TArray<TArray<FVector>> newScale;

	newLocation.SetNum(refSkeleton.GetNum());
	newRotation.SetNum(refSkeleton.GetNum());
	newScale.SetNum(refSkeleton.GetNum());

    for (int32 keyIndex = 0; keyIndex < numKeys; ++keyIndex)
    {
    	double time = animSequence->GetTimeAtFrame(keyIndex);

    	TArray<FTransform> boneWorldTransforms;
    	boneWorldTransforms.SetNum(refSkeleton.GetNum());

    	FTransform boneRootLocalTransform;
    	FAnimExtractContext extractBaseContext(static_cast<double>(time), animSequence->bEnableRootMotion);
    	animSequence->GetBoneTransform(boneRootLocalTransform, FSkeletonPoseBoneIndex(0), extractBaseContext, false);

    	boneWorldTransforms[0] = boneRootLocalTransform * offsetTransform;
    	FixedWorldPose(animSequence, 0, time, boneRootLocalTransform, boneWorldTransforms);  	
    	
    	for (int32 boneIndex = 0; boneIndex < refSkeleton.GetNum(); ++boneIndex)
    	{
    		const FKMBoneInfo& boneInfo = BoneInfos[boneIndex];
    		
    		FTransform parentBoneWorldTransform = boneInfo.ParentBoneIndex == INDEX_NONE ? offsetTransform : boneWorldTransforms[boneInfo.ParentBoneIndex]; 
			FTransform boneLocalTransform = boneWorldTransforms[boneIndex].GetRelativeTransform(parentBoneWorldTransform);

    		newLocation[boneIndex].Add(boneLocalTransform.GetLocation());
    		newRotation[boneIndex].Add(boneLocalTransform.GetRotation());
    		newScale[boneIndex].Add(boneLocalTransform.GetScale3D());
    	}
    }

	for (int32 boneIndex = 0; boneIndex < refSkeleton.GetNum(); ++boneIndex)
	{
		FName boneName = refSkeleton.GetBoneName(boneIndex);
		controller.SetBoneTrackKeys(boneName, newLocation[boneIndex], newRotation[boneIndex], newScale[boneIndex]);
	}
	
	controller.CloseBracket();
}

UKMBlendToAnimationModifier::UKMBlendToAnimationModifier() : Super()
{
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("Root"), false));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("Bip001-Pelvis"), false));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("Bip001-L-LegRoot"), true));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("Bip001-R-LegRoot"), true));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("lhip"), true));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("llegdir"), true));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("rhip"), true));
	BlendBones.Emplace(FKMBlendToAnimationBoneData::Create(TEXT("rlegdir"), true));
}

bool UKMBlendToAnimationModifier::IsBlendBone(const FReferenceSkeleton& refSkeleton, int32 boneIndex) const
{
	for (const FKMBlendToAnimationBoneData& blendBone : BlendBones)
	{
		const int32 blendBoneIndex = refSkeleton.FindBoneIndex(blendBone.BoneName);
		if (blendBoneIndex == INDEX_NONE)
		{
			continue;
		}

		if (boneIndex == blendBoneIndex)
		{
			return true;
		}

		if (!blendBone.bIsSubSearch)
		{
			continue;
		}
		
		if (refSkeleton.BoneIsChildOf(boneIndex, blendBoneIndex))
		{
			return true;
		}
	}

	return false;
}

void UKMBlendToAnimationModifier::OnApply_Implementation(UAnimSequence* animSequence)
{
    if (!IsValid(animSequence))
    {
        return;
    }
    
    if (!IsValid(TargetAnimationSequence))
    {
        return;
    }

    if (animSequence->GetSkeleton() != TargetAnimationSequence->GetSkeleton())
    {
        return;
    }

    if (BlendingTime <= 0.f)
    {
        return;
    }
    
    IAnimationDataController& controller = animSequence->GetController();
    const FReferenceSkeleton& refSkeleton = animSequence->GetSkeleton()->GetReferenceSkeleton();
    
    TArray<TArray<FVector>> newLocation;
    TArray<TArray<FQuat>> newRotation;
    TArray<TArray<FVector>> newScale;

    newLocation.SetNum(refSkeleton.GetNum());
    newRotation.SetNum(refSkeleton.GetNum());
    newScale.SetNum(refSkeleton.GetNum());

    TArray<FTransform> targetBoneTransform;
    for (int32 boneIndex = 0; boneIndex < refSkeleton.GetNum(); ++boneIndex)
    {
        FTransform boneTransform;
        FAnimExtractContext extractBaseContext(static_cast<double>(0.f), TargetAnimationSequence->bEnableRootMotion);

        TargetAnimationSequence->GetBoneTransform(boneTransform, FSkeletonPoseBoneIndex(boneIndex), extractBaseContext, false);

    	if (boneIndex == 1)
    	{
    		targetBoneTransform.Emplace(boneTransform * TargetRootTransform);
    	}
    	else
    	{
    	    targetBoneTransform.Emplace(boneTransform);
    	}
    }
	
	int32 addBoneIndex = 1;
	
	FTransform addSourceBoneTransform;
	FAnimExtractContext addExtractContext(static_cast<double>(AddTime), animSequence->bEnableRootMotion);
	animSequence->GetBoneTransform(addSourceBoneTransform, FSkeletonPoseBoneIndex(addBoneIndex), addExtractContext, false);

	FVector addLocation = targetBoneTransform[addBoneIndex].GetLocation() - addSourceBoneTransform.GetLocation();
	addLocation.Y = 0.f;
	FQuat addRotation = targetBoneTransform[addBoneIndex].GetRotation().Inverse() * addSourceBoneTransform.GetRotation();

	FTransform addBoneTransform = FTransform(addRotation, addLocation, FVector::One());

    const int32 numKeys = animSequence->GetNumberOfSampledKeys();

    for (int32 keyIndex = 0; keyIndex < numKeys; ++keyIndex)
    {
        const float time = static_cast<float>(animSequence->GetTimeAtFrame(keyIndex));
    	
        const float blendAlpha = FMath::IsNearlyZero(BlendingTime) ? 1.f : FMath::Clamp((time - BlendingStartTime) / BlendingTime, 0.f,1.f);

        FAnimExtractContext extractContext(static_cast<double>(time), animSequence->bEnableRootMotion);
        for (int32 boneIndex = 0; boneIndex < refSkeleton.GetNum(); ++boneIndex)
        {
        	if (!IsBlendBone(refSkeleton, boneIndex))
        	{
        		continue;
        	}
            FTransform sourceBoneTransform;
            animSequence->GetBoneTransform(sourceBoneTransform, FSkeletonPoseBoneIndex(boneIndex), extractContext, false);
            FTransform blendBoneTransform;
            blendBoneTransform.Blend(sourceBoneTransform,targetBoneTransform[boneIndex], blendAlpha * blendAlpha);

        	FTransform finalBoneTransform = bIsBlending ?  blendBoneTransform : sourceBoneTransform;

        	if (boneIndex == 1)
        	{
        		if (bIsAddTranslate)
        		{
        			finalBoneTransform.SetLocation(finalBoneTransform.GetLocation() + (addBoneTransform.GetLocation() * blendAlpha));
        		}
        		if (bIsAddRotation)
        		{
        			finalBoneTransform.SetRotation(finalBoneTransform.GetRotation() * (addBoneTransform.GetRotation() * blendAlpha));
        		}
        	}
        	
            newLocation[boneIndex].Emplace(finalBoneTransform.GetLocation());
            newRotation[boneIndex].Emplace(finalBoneTransform.GetRotation());
            newScale[boneIndex].Emplace(finalBoneTransform.GetScale3D());
        }
    }

	controller.OpenBracket(FText::FromString(TEXT("Blend To Animation")));
    for (int32 boneIndex = 0; boneIndex < refSkeleton.GetNum(); ++boneIndex)
    {
        const FName boneName = refSkeleton.GetBoneName(boneIndex);
    	if (!IsBlendBone(refSkeleton, boneIndex))
    	{
    		continue;
    	}
    	
		controller.AddBoneCurve(boneName);
        controller.SetBoneTrackKeys(boneName,newLocation[boneIndex], newRotation[boneIndex],newScale[boneIndex]);
    }

    controller.CloseBracket();
}