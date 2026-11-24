#include "SkeletalMeshConverter.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Animation/Skeleton.h"
#include "ReferenceSkeleton.h"
#include "StaticToSkeletalMeshConverter.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "UObject/Package.h"
#include "UObject/SavePackage.h"

#include "Misc/PackageName.h"
#include "Misc/Paths.h"

#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"

DEFINE_LOG_CATEGORY_STATIC(
	LogSkeletalMeshConverter,
	Log,
	All
);

static FAutoConsoleCommand
    GSkeletalMeshConverterCreateCommand(
        TEXT("SkeletalMeshConverter.Create"),
        TEXT("Create Skeleton Preview Mesh"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FSkeletalMeshConverter::Test
        )
    );

bool FSkeletalMeshConverter::ValidateReferenceSkeleton(
    const USkeleton* Skeleton,
    const USkeletalMesh* SkeletalMesh)
{
    if (!IsValid(Skeleton) ||
        !IsValid(SkeletalMesh))
    {
        return false;
    }

    if (SkeletalMesh->GetSkeleton() != Skeleton)
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Skeleton Reference Mismatch"));

        return false;
    }

    const FReferenceSkeleton& SourceRef =
        Skeleton->GetReferenceSkeleton();

    const FReferenceSkeleton& MeshRef =
        SkeletalMesh->GetRefSkeleton();

    const int32 NumBones =
        SourceRef.GetRawBoneNum();

    if (NumBones != MeshRef.GetRawBoneNum())
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Bone Count Mismatch : %d / %d"),
            NumBones,
            MeshRef.GetRawBoneNum());

        return false;
    }

    const TArray<FTransform>& SourcePose =
        SourceRef.GetRawRefBonePose();

    const TArray<FTransform>& MeshPose =
        MeshRef.GetRawRefBonePose();

    double MaxPositionError = 0.0;
    double MaxRotationError = 0.0;
    double MaxScaleError = 0.0;

    for (int32 BoneIndex = 0;
         BoneIndex < NumBones;
         ++BoneIndex)
    {
        const FName SourceName =
            SourceRef.GetBoneName(BoneIndex);

        const FName MeshName =
            MeshRef.GetBoneName(BoneIndex);

        // 1. Bone Name 검증

        if (SourceName != MeshName)
        {
            UE_LOG(LogSkeletalMeshConverter, Error,
                TEXT("Bone Name Mismatch : %d"),
                BoneIndex);

            return false;
        }

        // 2. Bone Hierarchy 검증

        if (SourceRef.GetParentIndex(BoneIndex) !=
            MeshRef.GetParentIndex(BoneIndex))
        {
            UE_LOG(LogSkeletalMeshConverter, Error,
                TEXT("Parent Index Mismatch : %s"),
                *SourceName.ToString());

            return false;
        }

        // 3. Reference Pose 검증

        const FTransform& SourceTransform =
            SourcePose[BoneIndex];

        const FTransform& MeshTransform =
            MeshPose[BoneIndex];

        const double PositionError =
            FVector::Distance(
                SourceTransform.GetLocation(),
                MeshTransform.GetLocation()
            );

        const double RotationError =
            FMath::RadiansToDegrees(
                SourceTransform.GetRotation()
                    .AngularDistance(
                        MeshTransform.GetRotation()
                    )
            );

        const double ScaleError =
            FVector::Distance(
                SourceTransform.GetScale3D(),
                MeshTransform.GetScale3D()
            );

        MaxPositionError =
            FMath::Max(
                MaxPositionError,
                PositionError
            );

        MaxRotationError =
            FMath::Max(
                MaxRotationError,
                RotationError
            );

        MaxScaleError =
            FMath::Max(
                MaxScaleError,
                ScaleError
            );

        if (PositionError > 0.001 ||
            RotationError > 0.01 ||
            ScaleError > 0.0001)
        {
            UE_LOG(LogSkeletalMeshConverter, Error,
                TEXT("Reference Transform Mismatch : %s"),
                *SourceName.ToString());

            return false;
        }

        UE_LOG(LogSkeletalMeshConverter, Log,
            TEXT("[PASS] Bone=%s Index=%d Parent=%d"),
            *SourceName.ToString(),
            BoneIndex,
            MeshRef.GetParentIndex(BoneIndex));
    }

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Validate Reference Skeleton Success"));

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Validated Bones : %d"),
        NumBones);

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Max Position Error : %.10f"),
        MaxPositionError);

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Max Rotation Error : %.10f"),
        MaxRotationError);

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Max Scale Error : %.10f"),
        MaxScaleError);

    return true;
}

USkeletalMesh* FSkeletalMeshConverter::CreatePreviewMesh(
    USkeleton* Skeleton,
    const FString& PackagePath,
    const FString& AssetName)
{
    if (!IsInGameThread() ||
        !IsValid(Skeleton))
    {
        return nullptr;
    }

    const FReferenceSkeleton& RefSkeleton =
        Skeleton->GetReferenceSkeleton();

    if (RefSkeleton.GetRawBoneNum() == 0)
        return nullptr;

    // ---------------------------------------
    // 1. Dummy StaticMesh 로딩
    // ---------------------------------------

    UStaticMesh* DummyMesh =
        LoadObject<UStaticMesh>(
            nullptr,
            TEXT("/Engine/BasicShapes/Cube.Cube")
        );

    if (!IsValid(DummyMesh))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Dummy StaticMesh Load Failed"));

        return nullptr;
    }

    // ---------------------------------------
    // 2. Package 생성
    // ---------------------------------------

    const FString LongPackageName =
        PackagePath / AssetName;

    if (!FPackageName::IsValidLongPackageName(
        LongPackageName))
    {
        return nullptr;
    }

    if (FindPackage(nullptr, *LongPackageName) ||
        FPackageName::DoesPackageExist(
            LongPackageName))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Asset Already Exists : %s"),
            *LongPackageName);

        return nullptr;
    }

    UPackage* Package =
        CreatePackage(*LongPackageName);

    if (!Package)
        return nullptr;

    // ---------------------------------------
    // 3. 신규 SkeletalMesh 생성
    // ---------------------------------------

    USkeletalMesh* NewMesh =
        NewObject<USkeletalMesh>(
            Package,
            FName(*AssetName),
            RF_Public |
            RF_Standalone |
            RF_Transactional
        );

    if (!NewMesh)
        return nullptr;

    auto AbortCreation =
        [&]() -> USkeletalMesh*
        {
            NewMesh->ClearFlags(
                RF_Public | RF_Standalone
            );

            NewMesh->MarkAsGarbage();

            Package->SetDirtyFlag(false);

            return nullptr;
        };

    // ---------------------------------------
    // 4. Skeleton 기반 SkeletalMesh 생성
    // ---------------------------------------

    const FName RootBoneName =
        RefSkeleton.GetBoneName(0);

    if (!FStaticToSkeletalMeshConverter::
        InitializeSkeletalMeshFromStaticMesh(
            NewMesh,
            DummyMesh,
            RefSkeleton,
            RootBoneName))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Initialize SkeletalMesh Failed"));

        return AbortCreation();
    }

    // ---------------------------------------
    // 5. 기존 C Skeleton 연결
    // ---------------------------------------

    NewMesh->SetSkeleton(Skeleton);

    // Inverse Reference Pose Matrix 생성

    NewMesh->CalculateInvRefMatrices();

    // ---------------------------------------
    // 6. Reference Skeleton 검증
    // ---------------------------------------

    if (!ValidateReferenceSkeleton(
        Skeleton,
        NewMesh))
    {
        return AbortCreation();
    }

    NewMesh->PostEditChange();

    // ---------------------------------------
    // 7. SkeletalMesh 저장
    // ---------------------------------------

    const FString FileName =
        FPackageName::LongPackageNameToFilename(
            LongPackageName,
            FPackageName::GetAssetPackageExtension()
        );

    IFileManager::Get().MakeDirectory(
        *FPaths::GetPath(FileName),
        true
    );

    NewMesh->MarkPackageDirty();

    FSavePackageArgs SaveArgs;

    SaveArgs.TopLevelFlags =
        RF_Public | RF_Standalone;

    SaveArgs.SaveFlags = SAVE_None;

    if (!UPackage::SavePackage(
        Package,
        NewMesh,
        *FileName,
        SaveArgs))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Save SkeletalMesh Failed"));

        return AbortCreation();
    }

    FAssetRegistryModule::AssetCreated(
        NewMesh
    );

    // ---------------------------------------
    // 8. Skeleton Preview Mesh 설정
    // ---------------------------------------

    Skeleton->Modify();

    Skeleton->SetPreviewMesh(
        NewMesh,
        true
    );

    Skeleton->MarkPackageDirty();

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("================================"));

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Create Preview Mesh Success"));

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Skeleton : %s"),
        *Skeleton->GetName());

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("SkeletalMesh : %s"),
        *NewMesh->GetName());

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Bone Count : %d"),
        NewMesh->GetRefSkeleton().GetRawBoneNum());

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("Saved : %s"),
        *FileName);

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("================================"));

    return NewMesh;
}

void FSkeletalMeshConverter::Test(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogSkeletalMeshConverter, Warning,
            TEXT("Usage: SkeletalMeshConverter.Create ")
            TEXT("SkeletonPath OutputFolder AssetName"));

        return;
    }

    USkeleton* Skeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    if (!IsValid(Skeleton))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Skeleton Load Failed"));

        return;
    }

    USkeletalMesh* NewMesh =
        CreatePreviewMesh(
            Skeleton,
            Args[1],
            Args[2]
        );

    if (!IsValid(NewMesh))
    {
        UE_LOG(LogSkeletalMeshConverter, Error,
            TEXT("Create Preview Mesh Test Failed"));

        return;
    }

    UE_LOG(LogSkeletalMeshConverter, Log,
        TEXT("SkeletalMesh Converter Test Success"));
}