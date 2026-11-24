#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameRate.h"

#include "SkeletonConverter.h"

class UAnimSequence;
class USkeleton;

// ---------------------------------------
// Animation Bone Track
// ---------------------------------------

struct FAnimationConverterTrack
{
	FName BoneName = NAME_None;

	int32 BoneIndex = INDEX_NONE;

	// 원본 Local Transform Keys
	TArray<FTransform> Keys;
};

struct FAnimationConverterBoneMapping
{
	int32 SourceBoneIndex = INDEX_NONE;

	int32 TargetBoneIndex = INDEX_NONE;

	FName SourceBoneName = NAME_None;

	FName TargetBoneName = NAME_None;

	// Component Space 기준 회전축 차이
	FQuat AxisOffset = FQuat::Identity;
};


struct FAnimationConverterMapping
{
	TArray<FSkeletonConverterBone> TargetReferenceBones;

	// Target Bone Index 순서로 저장
	TArray<FAnimationConverterBoneMapping> BoneMappings;
};

// ---------------------------------------
// Animation Data
// ---------------------------------------

struct FAnimationConverterData
{
	FName AnimationName = NAME_None;

	FString AssetPath;

	FFrameRate FrameRate = FFrameRate(30, 1);

	int32 NumFrames = 0;

	int32 NumKeys = 0;

	double PlayLength = 0.0;

	bool bEnableRootMotion = false;

	// 원본 Skeleton Reference Pose
	TArray<FSkeletonConverterBone> ReferenceBones;

	// 원본 Animation Tracks
	TArray<FAnimationConverterTrack> Tracks;

	// Skeleton Bone Index -> Animation Track Index
	TArray<int32> BoneToTrackIndex;
};

// ---------------------------------------
// Animation Converter
// ---------------------------------------

class FAnimationConverter
{
public:

	static bool ExtractAnimationData(
		const UAnimSequence* Animation,
		FAnimationConverterData& OutData
	);

	static bool EvaluateSourcePose(
		const FAnimationConverterData& Data,
		int32 FrameIndex,
		TArray<FSkeletonConverterBone>& OutBones
	);

	static void DumpAnimationData(
		const FAnimationConverterData& Data
	);

	static bool ExportJson(
		const FAnimationConverterData& Data,
		const FString& FilePath
	);
	
public:
	static void TestDrawPose(
		const TArray<FString>& Args
	);
	
	static bool BuildAnimationMapping(
		const FAnimationConverterData& SourceData,
		const USkeleton* TargetSkeleton,
		FAnimationConverterMapping& OutMapping
	);

	static bool ConvertAnimationPose(
		const TArray<FSkeletonConverterBone>& SourcePose,
		const FAnimationConverterMapping& Mapping,
		TArray<FSkeletonConverterBone>& OutTargetPose
	);

	static bool ConvertAnimationData(
		const FAnimationConverterData& SourceData,
		const USkeleton* TargetSkeleton,
		FAnimationConverterData& OutData
	);

	static bool ValidateConvertedPose(
		const TArray<FSkeletonConverterBone>& SourcePose,
		const TArray<FSkeletonConverterBone>& TargetPose,
		const FAnimationConverterMapping& Mapping
	);
	
	static TConstArrayView<FSkeletonConverterMapping> GetBoneMapping();
	
	static UAnimSequence* CreateAnimSequenceAsset(
		const FAnimationConverterData& Data,
		USkeleton* TargetSkeleton,
		const FString& PackagePath,
		const FString& AssetName
	);

	static bool ValidateAnimationAsset(
		const UAnimSequence* Animation,
		const FAnimationConverterData& ExpectedData
	);

	static void TestCreate(
		const TArray<FString>& Args
	);

	static void TestConvert(
		const TArray<FString>& Args
	);

	static void TestDrawConvertedPose(
		const TArray<FString>& Args
	);

	static void Test(
		const TArray<FString>& Args
	);
};