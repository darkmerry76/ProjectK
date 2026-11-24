#pragma once

#include "CoreMinimal.h"

class USkeleton;
struct FReferenceSkeleton;

struct FSkeletonConverterBone
{
    int32 BoneIndex = INDEX_NONE;
    int32 ParentIndex = INDEX_NONE;

    FName BoneName = NAME_None;
    FName ParentName = NAME_None;

    FTransform LocalTransform = FTransform::Identity;
    FTransform ComponentTransform = FTransform::Identity;
};

struct FSkeletonConverterMapping
{
    FName SourceBone;
    FName TargetBone;
};

struct FSkeletonAxisMapping
{
    FName BoneName;
    FName DirectionBone;
    bool bUseParentDirection;
};

struct FSkeletonPalmFrame
{
    FVector Forward = FVector::ZeroVector;
    FVector Across = FVector::ZeroVector;
    FVector Normal = FVector::ZeroVector;

    FQuat Rotation = FQuat::Identity;
};

class FSkeletonConverter
{
public:

    static bool GetBoneData(
        const USkeleton* Skeleton,
        TArray<FSkeletonConverterBone>& OutBones
    );

    static void DumpSkeleton(
        const USkeleton* Skeleton
    );

    static bool ExportJson(
        const USkeleton* Skeleton,
        const FString& FilePath
    );
    
    static bool ConvertHierarchy(
        const USkeleton* SourceSkeleton,
        const USkeleton* ReferenceSkeleton,
        TArray<FSkeletonConverterBone>& OutBones
    );

    static bool ValidateHierarchy(
        const TArray<FSkeletonConverterBone>& Bones
    );
    
    static bool ConvertBoneAxis(
    const USkeleton* ReferenceSkeleton,
    TArray<FSkeletonConverterBone>& InOutBones);
    
    static bool BuildReferenceSkeleton(
    const TArray<FSkeletonConverterBone>& Bones,
    FReferenceSkeleton& OutRefSkeleton);

    static USkeleton* CreateSkeletonAsset(
        const TArray<FSkeletonConverterBone>& Bones,
        const FString& PackagePath,
        const FString& AssetName);
    
    static bool CorrectBoneTwist(
        const USkeleton* ReferenceSkeleton,
        TArray<FSkeletonConverterBone>& InOutBones
    );
    
    static void DrawDebugSkeleton(
        UWorld* World,
        const USkeleton* Skeleton,
        const FTransform& WorldTransform,
        bool bDrawBoneNames = false,
        bool bDrawBoneAxes = true,
        float AxisSize = 5.0f,
        float Duration = 60.0f,
        FColor BoneColor = FColor::White
    );

    static void TestDebugDraw();

    static void ClearDebugDraw();
    
    static void TestBoneRotation(
        const FName& BoneName,
        const FVector& LocalAxis,
        float Angle
    );
    
    static void Test();
    
public:
    static TConstArrayView<FSkeletonConverterMapping> GetBoneMapping();
    static bool ApplyBoneRotation(
        TArray<FSkeletonConverterBone>& InOutBones,
        const FName& BoneName,
        const FVector& LocalAxis,
        float Angle
    );

    static void DrawDebugSkeletonBones(
        UWorld* World,
        const USkeleton* Skeleton,
        const TArray<FSkeletonConverterBone>& Bones,
        const FTransform& WorldTransform,
        bool bDrawBoneNames = false,
        bool bDrawBoneAxes = true,
        float AxisSize = 5.0f,
        float Duration = -1.0f,
        FColor BoneColor = FColor::White
    );
    
    static USkeleton* Convert(
        const USkeleton* SkeletonA,
        const USkeleton* SkeletonB,
        const FString& OutputFolder,
        const FString& AssetName);
    
    static void CommandConvert(
        const TArray<FString>& Args);

    static void TestBoneRotation(
        const TArray<FString>& Args
    );
};