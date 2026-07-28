#include "KMAnimModifier_Bone.h"

#include "SkeletalMeshAttributes.h"

void UKMAnimModifier_Bone::OnApply_Implementation(UAnimSequence* AnimSeq)
{
	if (!AnimSeq) return;

	IAnimationDataController& Controller = AnimSeq->GetController();
	
	const float Duration = AnimSeq->GetPlayLength();
	const int32 NumKeys = AnimSeq->GetNumberOfSampledKeys();

	TArray<FVector> PosKeys;
	TArray<FQuat> RotKeys;
	TArray<FVector> ScaleKeys;

	int32 boneIndex = AnimSeq->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}

	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);

	for (int32 i = 0; i < NumKeys; ++i)
	{
		double Time = AnimSeq->GetTimeAtFrame(i);

		FTransform OutTransform;
		FAnimExtractContext extractContext(Time, AnimSeq->bEnableRootMotion);
		AnimSeq->GetBoneTransform(OutTransform, poseBoneIndex, extractContext, false);

		FVector CurrentPos = OutTransform.GetLocation();
		FRotator CurrentRot = OutTransform.GetRotation().Rotator();
		FVector CurrentScale = OutTransform.GetScale3D();

		float CurveVal = AnimSeq->EvaluateCurveData(TargetCurveName, Time);

		double* resultValue = nullptr;

		switch (Type)
		{
		case EKM_BoneTransformType::TRANS_X: resultValue = &CurrentPos.X;break;
		case EKM_BoneTransformType::TRANS_Y: resultValue = &CurrentPos.Y;break;
		case EKM_BoneTransformType::TRANS_Z: resultValue = &CurrentPos.Z;break;
		case EKM_BoneTransformType::ROTATION_X: resultValue = &CurrentRot.Roll;break;
		case EKM_BoneTransformType::ROTATION_Y: resultValue = &CurrentRot.Pitch;break;
		case EKM_BoneTransformType::ROTATION_Z: resultValue = &CurrentRot.Yaw;break;
		case EKM_BoneTransformType::SCALE_X: resultValue = &CurrentScale.X;break;
		case EKM_BoneTransformType::SCALE_Y: resultValue = &CurrentScale.Y;break;
		case EKM_BoneTransformType::SCALE_Z: resultValue = &CurrentScale.Z;break;
		default:break;
		}

		if (resultValue)
		{
			switch (OperatorType)
			{
			case EKM_OperatorType::Set:(*resultValue) = CurveVal; break;
			case EKM_OperatorType::Add:(*resultValue) += CurveVal; break;
			case EKM_OperatorType::Subtract:(*resultValue) -= CurveVal; break;
			case EKM_OperatorType::Multiply:(*resultValue) *= CurveVal; break;
			case EKM_OperatorType::Divide:(*resultValue) /= CurveVal; break;
			default:break;
			}
		}

		PosKeys.Add(CurrentPos);
		RotKeys.Add(CurrentRot.Quaternion());
		ScaleKeys.Add(CurrentScale);
	}
	Controller.SetBoneTrackKeys(BoneName, PosKeys, RotKeys, ScaleKeys);
}

void UKMAnimModifier_BoneToRoot::OnApply_Implementation(UAnimSequence* AnimSeq)
{
	if (!AnimSeq) return;

	IAnimationDataController& Controller = AnimSeq->GetController();
	
	const float Duration = AnimSeq->GetPlayLength();
	const int32 NumKeys = AnimSeq->GetNumberOfSampledKeys();

	TArray<FVector> BonePosKeys;
	TArray<FQuat> BoneRotKeys;
	TArray<FVector> BoneScaleKeys;

	TArray<FVector> RootPosKeys;
	TArray<FQuat> RootRotKeys;
	TArray<FVector> RootScaleKeys;
	
	int32 rootboneIndex = 0;
	int32 boneIndex = AnimSeq->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}

	FSkeletonPoseBoneIndex poseRootIndex(0);
	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);

	for (int32 i = 0; i < NumKeys; ++i)
	{
		double Time = AnimSeq->GetTimeAtFrame(i);

		FTransform OutBoneTransform;
		FTransform OutBaseTransform;
		FTransform OutRootTransform;
		FAnimExtractContext extractBaseContext(static_cast<double>(0.f), AnimSeq->bEnableRootMotion);
		AnimSeq->GetBoneTransform(OutBaseTransform, poseBoneIndex, extractBaseContext, false);

		FAnimExtractContext extractContext(Time, AnimSeq->bEnableRootMotion);
		AnimSeq->GetBoneTransform(OutBoneTransform, poseBoneIndex, extractContext, false);
		AnimSeq->GetBoneTransform(OutRootTransform, poseRootIndex, extractContext, false);

		FVector Offset = OutBoneTransform.GetLocation() - OutBaseTransform.GetLocation();
		FVector CurrentRootPos = FVector(CopyTranslateX ? Offset.X : OutRootTransform.GetLocation().X,
		CopyTranslateY ? Offset.Y : OutRootTransform.GetLocation().Y,
		CopyTranslateZ ? Offset.Z : OutRootTransform.GetLocation().Z);
		FRotator CurrentRootRot = OutRootTransform.GetRotation().Rotator();
		FVector CurrentRootScale = OutRootTransform.GetScale3D();
		
		RootRotKeys.Add(CurrentRootRot.Quaternion());
		RootScaleKeys.Add(CurrentRootScale);

		float xScale = X > 0 ? 1.f : -1.f;
		float yScale = Y > 0 ? 1.f : -1.f;
		float zScale = Z > 0 ? 1.f : -1.f;
		RootPosKeys.Add(FVector((&CurrentRootPos.X)[abs(X)] * xScale, (&CurrentRootPos.X)[abs(Y)] * yScale, (&CurrentRootPos.X)[abs(Z)] * zScale));
		//RootPosKeys.Add(FVector(OutRootTransform.GetLocation().X, OutRootTransform.GetLocation().Y, OutRootTransform.GetLocation().Z));
		
		FVector CurrentBonePos = FVector(DefaultTranslateX ? DefaultTranslate.X : OutBoneTransform.GetLocation().X,
		DefaultTranslateY ? DefaultTranslate.Y : OutBoneTransform.GetLocation().Y,
		DefaultTranslateZ ? DefaultTranslate.Z : OutBoneTransform.GetLocation().Z);
		FRotator CurrentBoneRot = OutBoneTransform.GetRotation().Rotator();
		FVector CurrentBoneScale = OutBoneTransform.GetScale3D();

		BonePosKeys.Add(CurrentBonePos);
		BoneRotKeys.Add(CurrentBoneRot.Quaternion());
		BoneScaleKeys.Add(CurrentBoneScale);
	}
	Controller.SetBoneTrackKeys(BoneName, BonePosKeys, BoneRotKeys, BoneScaleKeys);
	Controller.SetBoneTrackKeys(TEXT("Root"), RootPosKeys, RootRotKeys, RootScaleKeys);
}

void UKMAnimModifier_BoneTransform::OnApply_Implementation(UAnimSequence* AnimSeq)
{
	if (!AnimSeq) return;

	IAnimationDataController& Controller = AnimSeq->GetController();
	
	const int32 NumKeys = AnimSeq->GetNumberOfSampledKeys();

	TArray<FVector> BonePosKeys;
	TArray<FQuat> BoneRotKeys;
	TArray<FVector> BoneScaleKeys;

	TArray<FVector> RootPosKeys;
	TArray<FQuat> RootRotKeys;
	TArray<FVector> RootScaleKeys;
	
	int32 boneIndex = AnimSeq->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (boneIndex == INDEX_NONE)
	{
		return;
	}
	FSkeletonPoseBoneIndex poseBoneIndex(boneIndex);

	for (int32 i = 0; i < NumKeys; ++i)
	{
		double Time = AnimSeq->GetTimeAtFrame(i);

		FTransform OutBoneTransform;
		FAnimExtractContext extractContext(Time, AnimSeq->bEnableRootMotion);
		AnimSeq->GetBoneTransform(OutBoneTransform, poseBoneIndex, extractContext, false);

		FVector CurrentBonePos = OutBoneTransform.GetLocation() + AddTransform;

		FQuat addQuat = AddRotator.Quaternion();
		FQuat CurrentBoneRot = addQuat * OutBoneTransform.GetRotation();
		
		FVector CurrentBoneScale = OutBoneTransform.GetScale3D();
		
		BonePosKeys.Add(CurrentBonePos);
		BoneRotKeys.Add(CurrentBoneRot);
		BoneScaleKeys.Add(CurrentBoneScale);
	}
	Controller.SetBoneTrackKeys(BoneName, BonePosKeys, BoneRotKeys, BoneScaleKeys);
}

void UKMAnimModifier_FrameCut::OnApply_Implementation(UAnimSequence* animationSequence)
{
/*	if(animationSequence == nullptr)
	{
		return;
	}

	const IAnimationDataModel* dataModel = animationSequence->GetDataModel();
	const int32 totalFrames = dataModel->GetNumberOfFrames();
	if (totalFrames <= 1)
	{
		return;
	}

	// Clamp
	const int32 startFrame = FMath::Clamp(static_cast<int32>(StartFrame), 0, totalFrames - 1);
	const int32 endFrame   = FMath::Clamp(static_cast<int32>(EndFrame), startFrame + 1, totalFrames);

	const float sequenceLength = animationSequence->GetPlayLength();
	const float frameTime = sequenceLength / (totalFrames - 1);

	const float startTime = startFrame * frameTime;
	const float endTime   = endFrame * frameTime;
	const float newLength = endTime - startTime;

	if (newLength <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// Raw Data 접근
	TArray<FRawAnimSequenceTrack>& rawTracks = animationSequence->GetRawAnimationData();

	const int32 newFrameCount = endFrame - startFrame;

	for (FRawAnimSequenceTrack& track : rawTracks)
	{
		// Pos
		if (track.PosKeys.Num() > 0)
		{
			TArray<FVector> newPosKeys;
			newPosKeys.Reserve(newFrameCount);

			for (int32 i = startFrame; i < endFrame; ++i)
			{
				newPosKeys.Add(track.PosKeys[i]);
			}

			track.PosKeys = MoveTemp(newPosKeys);
		}

		// Rot
		if (track.RotKeys.Num() > 0)
		{
			TArray<FQuat> newRotKeys;
			newRotKeys.Reserve(newFrameCount);

			for (int32 i = startFrame; i < endFrame; ++i)
			{
				newRotKeys.Add(track.RotKeys[i]);
			}

			track.RotKeys = MoveTemp(newRotKeys);
		}

		// Scale
		if (track.ScaleKeys.Num() > 0)
		{
			TArray<FVector> newScaleKeys;
			newScaleKeys.Reserve(newFrameCount);

			for (int32 i = startFrame; i < endFrame; ++i)
			{
				newScaleKeys.Add(track.ScaleKeys[i]);
			}

			track.ScaleKeys = MoveTemp(newScaleKeys);
		}
	}

	// Frame 수 재설정
	animationSequence->SetRawNumberOfFrame(newFrameCount);

	// 길이 재설정
	animationSequence->SequenceLength = newLength;

	// Curve도 같이 잘라줘야함 (중요)
	animationSequence->GetController().OpenBracket(TEXT("FrameCut"));

	{
		IAnimationDataController& controller = animationSequence->GetController();

		controller.SetPlayLength(newLength);

		// Curve 리맵
		for (FFloatCurve& curve : animationSequence->GetCurveData().FloatCurves)
		{
			TArray<FRichCurveKey> newKeys;

			for (auto& key : curve.FloatCurve.GetConstRefOfKeys())
			{
				if (key.Time >= startTime && key.Time <= endTime)
				{
					FRichCurveKey newKey = key;
					newKey.Time -= startTime; // 시간 재정렬
					newKeys.Add(newKey);
				}
			}

			curve.FloatCurve.Reset();
			for (const FRichCurveKey& k : newKeys)
			{
				curve.FloatCurve.AddKey(k);
			}
		}
	}

	animationSequence->GetController().CloseBracket();

	// Notify도 잘라줘야함
	TArray<FAnimNotifyEvent>& notifies = animationSequence->Notifies;
	for (int32 i = notifies.Num() - 1; i >= 0; --i)
	{
		FAnimNotifyEvent& notify = notifies[i];

		if (notify.GetTime() < startTime || notify.GetTime() > endTime)
		{
			notifies.RemoveAt(i);
		}
		else
		{
			notify.SetTime(notify.GetTime() - startTime);
		}
	}

	// 필수
	animationSequence->MarkRawDataAsModified();
	animationSequence->PostEditChange();*/
}

void UKMFixPelvisYawModifier::CreateBoneInfo(const UAnimSequence* animationSequence, TArray<FKMBoneInfo>& outBoneInfos)
{
	const FReferenceSkeleton& RefSkeleton = animationSequence->GetSkeleton()->GetReferenceSkeleton();

	const int32 numBones = RefSkeleton.GetNum();
	outBoneInfos.SetNum(numBones);
	for (int32 boneIndex = 0; boneIndex < numBones; ++boneIndex)
	{
		int32 parentBoneIndex = RefSkeleton.GetParentIndex(boneIndex);
		if (parentBoneIndex != INDEX_NONE)
		{
			outBoneInfos[parentBoneIndex].ChildBoneIndices.Emplace(boneIndex);
		}
		outBoneInfos[boneIndex].ParentBoneIndex = parentBoneIndex;
		outBoneInfos[boneIndex].BoneName = RefSkeleton.GetBoneName(boneIndex);
	}
}

void UKMFixPelvisYawModifier::FixedWorldPose(UAnimSequence* animationSequence, int32 boneIndex, float time, const FTransform& parentTransform, TArray<FTransform>& outBoneWorldTransforms)
{
	for (auto childBoneIndex : BoneInfos[boneIndex].ChildBoneIndices)
	{
		FAnimExtractContext extractBaseContext(static_cast<double>(time), animationSequence->bEnableRootMotion);

		FTransform boneLocalTransform;
		animationSequence->GetBoneTransform(boneLocalTransform, FSkeletonPoseBoneIndex(childBoneIndex), extractBaseContext, false);
		outBoneWorldTransforms[childBoneIndex] = boneLocalTransform * parentTransform;

		FixedWorldPose(animationSequence, childBoneIndex, time, outBoneWorldTransforms[childBoneIndex], outBoneWorldTransforms);
	}
}

void UKMFixPelvisYawModifier::OnApply_Implementation(UAnimSequence* animationSequence)
{
	CreateBoneInfo(animationSequence, BoneInfos);
	
    IAnimationDataController& controller = animationSequence->GetController();
    const FReferenceSkeleton& refSkeleton = animationSequence->GetSkeleton()->GetReferenceSkeleton();
	
    FTransform offsetTransform(OffsetRotator);
	
    controller.OpenBracket(FText::FromString(TEXT("Perfect Matrix Inverse Rotation")));

	const int32 numKeys = animationSequence->GetNumberOfSampledKeys();

	TArray<TArray<FVector>> newLocation;
	TArray<TArray<FQuat>> newRotation;
	TArray<TArray<FVector>> newScale;

	newLocation.SetNum(refSkeleton.GetNum());
	newRotation.SetNum(refSkeleton.GetNum());
	newScale.SetNum(refSkeleton.GetNum());

    for (int32 keyIndex = 0; keyIndex < numKeys; ++keyIndex)
    {
    	double time = animationSequence->GetTimeAtFrame(keyIndex);

    	TArray<FTransform> boneWorldTransforms;
    	boneWorldTransforms.SetNum(refSkeleton.GetNum());

    	FTransform boneRootLocalTransform;
    	FAnimExtractContext extractBaseContext(static_cast<double>(time), animationSequence->bEnableRootMotion);
    	animationSequence->GetBoneTransform(boneRootLocalTransform, FSkeletonPoseBoneIndex(0), extractBaseContext, false);

    	boneWorldTransforms[0] = boneRootLocalTransform * offsetTransform;
    	FixedWorldPose(animationSequence, 0, time, boneRootLocalTransform, boneWorldTransforms);  	
    	
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