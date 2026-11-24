#include "CharacterConverter.h"

#include "SkeletonConverter.h"
#include "SkeletalMeshConverter.h"
#include "AnimationConverter.h"

#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/ARFilter.h"

#include "Modules/ModuleManager.h"

#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"

#include "Misc/PackageName.h"
#include "Misc/ScopedSlowTask.h"

#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"

DEFINE_LOG_CATEGORY_STATIC(
    LogCharacterConverter,
    Log,
    All
);

static FAutoConsoleCommand
    GCharacterConverterCommand(
        TEXT("CharacterConverter.Convert"),
        TEXT("Convert Skeleton and All Animations"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FCharacterConverter::CommandConvert
        )
    );

static FAutoConsoleCommand
    GCharacterConverterAnimationsCommand(
        TEXT("CharacterConverter.Animations"),
        TEXT("Convert All Animations To Existing Skeleton"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FCharacterConverter::CommandConvertAnimations
        )
    );

static FAutoConsoleCommand
    GCharacterConverterListCommand(
        TEXT("CharacterConverter.List"),
        TEXT("List All Animations Using Source Skeleton"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FCharacterConverter::CommandListAnimations
        )
    );

static FAutoConsoleCommand
    GCharacterConverterConvertAllCommand(
        TEXT("CharacterConverter.ConvertAll"),
        TEXT("Convert Skeleton, SkeletalMesh and All Animations"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FCharacterConverter::CommandConvertAll
        )
    );

bool FCharacterConverter::FindAnimations(
    const USkeleton* SourceSkeleton,
    TArray<FAssetData>& OutAssets)
{
    OutAssets.Reset();

    if (!IsValid(SourceSkeleton))
        return false;

    // 1. Asset Registry

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<
            FAssetRegistryModule>(
                TEXT("AssetRegistry")
            );

    IAssetRegistry& AssetRegistry =
        AssetRegistryModule.Get();

    // 초기 Asset 검색 완료 대기

    AssetRegistry.SearchAllAssets(true);
    AssetRegistry.WaitForCompletion();

    // 2. AnimSequence 검색

    FARFilter Filter;

    Filter.ClassPaths.Add(
        UAnimSequence::StaticClass()
            ->GetClassPathName()
    );

    Filter.PackagePaths.Add(
        FName(TEXT("/Game"))
    );

    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = false;

    TArray<FAssetData> AnimationAssets;

    if (!AssetRegistry.GetAssets(
        Filter,
        AnimationAssets))
    {
        return false;
    }

    // 3. Source Skeleton 일치 여부 확인

    const FString SourceSkeletonPath =
        SourceSkeleton->GetPathName();

    static const FName SkeletonTag(
        TEXT("Skeleton")
    );

    for (const FAssetData& Asset :
         AnimationAssets)
    {
        FString SkeletonTagValue;

        if (Asset.GetTagValue(
                SkeletonTag,
                SkeletonTagValue) &&
            !SkeletonTagValue.IsEmpty() &&
            SkeletonTagValue != TEXT("None"))
        {
            // ExportTextPath -> ObjectPath

            const FString ObjectPath =
                FPackageName::ExportTextPathToObjectPath(
                    SkeletonTagValue
                );

            if (ObjectPath == SourceSkeletonPath)
            {
                OutAssets.Add(Asset);
            }
        }
        else
        {
            // Skeleton Tag가 없는 에셋은 직접 확인

            UAnimSequence* Animation =
                Cast<UAnimSequence>(
                    Asset.GetAsset()
                );

            if (IsValid(Animation) &&
                Animation->GetSkeleton() ==
                    SourceSkeleton)
            {
                OutAssets.Add(Asset);
            }
        }
    }

    // 4. 정렬

    OutAssets.Sort(
        [](const FAssetData& A,
           const FAssetData& B)
        {
            return
                A.GetSoftObjectPath().ToString() <
                B.GetSoftObjectPath().ToString();
        }
    );

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Source Skeleton : %s"),
        *SourceSkeletonPath);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Found AnimSequences : %d"),
        OutAssets.Num());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    return true;
}

bool FCharacterConverter::ConvertAllAnimations(
    USkeleton* SourceSkeleton,
    USkeleton* TargetSkeleton,
    const FString& OutputFolder,
    FCharacterConverterResult& OutResult)
{
    OutResult = FCharacterConverterResult();

    if (!IsInGameThread() ||
        !IsValid(SourceSkeleton) ||
        !IsValid(TargetSkeleton) ||
        SourceSkeleton == TargetSkeleton)
    {
        return false;
    }

    const double StartTime =
        FPlatformTime::Seconds();

    // 1. 관련 Animation 검색

    TArray<FAssetData> AnimationAssets;

    if (!FindAnimations(
        SourceSkeleton,
        AnimationAssets))
    {
        return false;
    }

    OutResult.TotalCount =
        AnimationAssets.Num();

    // 2. Progress Dialog

    FScopedSlowTask SlowTask(
        static_cast<float>(AnimationAssets.Num()),
        FText::FromString(
            TEXT("Converting Animations")
        )
    );

    SlowTask.MakeDialog(false);

    // 3. 전체 Animation 변환

    for (int32 AssetIndex = 0;
         AssetIndex < AnimationAssets.Num();
         ++AssetIndex)
    {
        const FAssetData& Asset =
            AnimationAssets[AssetIndex];

        const FString AssetName =
            TEXT("Converted_") + Asset.AssetName.ToString();

        const FString SourcePath =
            Asset.GetSoftObjectPath().ToString();

        SlowTask.EnterProgressFrame(
            1.0f,
            FText::FromString(AssetName)
        );

        UE_LOG(LogCharacterConverter, Log,
            TEXT("[%d / %d] Convert : %s"),
            AssetIndex + 1,
            AnimationAssets.Num(),
            *AssetName);

        // -----------------------------------
        // 4. Output Folder 계산
        // -----------------------------------

        FString RelativePath =
            Asset.PackagePath.ToString();

        RelativePath.RemoveFromStart(
            TEXT("/Game")
        );

        RelativePath.RemoveFromStart(
            TEXT("/")
        );

        FString AnimationFolder =
            OutputFolder / TEXT("Animations");

        if (!RelativePath.IsEmpty())
        {
            AnimationFolder =
                AnimationFolder / RelativePath;
        }

        const FString TargetPackageName =
            AnimationFolder / AssetName;

        // -----------------------------------
        // 5. 기존 에셋 확인
        // -----------------------------------

        if (FindPackage(
                nullptr,
                *TargetPackageName) ||

            FPackageName::DoesPackageExist(
                TargetPackageName))
        {
            ++OutResult.SkippedCount;

            OutResult.SkippedAssets.Add(
                SourcePath + TEXT(" : Already Exists")
            );

            UE_LOG(LogCharacterConverter, Warning,
                TEXT("[SKIP] Already Exists : %s"),
                *TargetPackageName);

            continue;
        }

        // -----------------------------------
        // 6. Source AnimSequence 로딩
        // -----------------------------------

        TStrongObjectPtr<UAnimSequence> SourceAnimation(
            Cast<UAnimSequence>(
                Asset.GetAsset()
            )
        );

        if (!IsValid(SourceAnimation.Get()))
        {
            ++OutResult.FailedCount;

            OutResult.FailedAssets.Add(
                SourcePath + TEXT(" : Load Failed")
            );

            continue;
        }

        // Asset Registry 검색 결과 재검증

        if (SourceAnimation->GetSkeleton() !=
            SourceSkeleton)
        {
            ++OutResult.SkippedCount;

            OutResult.SkippedAssets.Add(
                SourcePath + TEXT(" : Skeleton Mismatch")
            );

            continue;
        }

        // Additive Animation 제외

        if (SourceAnimation->IsValidAdditive())
        {
            ++OutResult.SkippedCount;

            OutResult.SkippedAssets.Add(
                SourcePath + TEXT(" : Additive")
            );

            continue;
        }

        // -----------------------------------
        // 7. Source Animation Data 추출
        // -----------------------------------

        FAnimationConverterData SourceData;

        if (!FAnimationConverter::ExtractAnimationData(
            SourceAnimation.Get(),
            SourceData))
        {
            ++OutResult.FailedCount;

            OutResult.FailedAssets.Add(
                SourcePath + TEXT(" : Extract Failed")
            );

            continue;
        }

        // -----------------------------------
        // 8. Animation 변환
        // -----------------------------------

        FAnimationConverterData TargetData;

        if (!FAnimationConverter::ConvertAnimationData(
            SourceData,
            TargetSkeleton,
            TargetData))
        {
            ++OutResult.FailedCount;

            OutResult.FailedAssets.Add(
                SourcePath + TEXT(" : Convert Failed")
            );

            continue;
        }

        // -----------------------------------
        // 9. AnimSequence 생성 및 저장
        // -----------------------------------

        UAnimSequence* NewAnimation =
            FAnimationConverter::CreateAnimSequenceAsset(
                TargetData,
                TargetSkeleton,
                AnimationFolder,
                AssetName
            );

        if (!IsValid(NewAnimation))
        {
            ++OutResult.FailedCount;

            OutResult.FailedAssets.Add(
                SourcePath + TEXT(" : Create Failed")
            );

            continue;
        }

        // -----------------------------------
        // 10. 성공
        // -----------------------------------

        ++OutResult.SuccessCount;

        UE_LOG(LogCharacterConverter, Log,
            TEXT("[SUCCESS] %s"),
            *NewAnimation->GetPathName());
    }

    OutResult.ElapsedTime =
        FPlatformTime::Seconds() -
        StartTime;

    DumpResult(OutResult);

    return OutResult.FailedCount == 0;
}

bool FCharacterConverter::ConvertCharacter(
    USkeleton* SourceSkeleton,
    USkeleton* ReferenceSkeleton,
    const FString& OutputFolder)
{
    if (!IsInGameThread() ||
        !IsValid(SourceSkeleton) ||
        !IsValid(ReferenceSkeleton) ||
        SourceSkeleton == ReferenceSkeleton)
    {
        return false;
    }

    const double StartTime =
        FPlatformTime::Seconds();

    // ---------------------------------------
    // 1. Asset Name 설정
    // ---------------------------------------

    FString BaseName = SourceSkeleton->GetName();
    BaseName.RemoveFromEnd(TEXT("_Skeleton"));

    const FString SkeletonAssetName =
        TEXT("Converted_") + BaseName + TEXT("_Skeleton");

    const FString MeshAssetName =
        TEXT("Converted_") + BaseName;

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Character Conversion Start"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Source : %s"),
        *SourceSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Reference : %s"),
        *ReferenceSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Output : %s"),
        *OutputFolder);

    // ---------------------------------------
    // 2. Skeleton Hierarchy 변환
    // ---------------------------------------

    TArray<FSkeletonConverterBone> ConvertedBones;

    if (!FSkeletonConverter::ConvertHierarchy(
        SourceSkeleton,
        ReferenceSkeleton,
        ConvertedBones))
    {
        return false;
    }

    // ---------------------------------------
    // 3. Bone Axis 변환
    // ---------------------------------------

    if (!FSkeletonConverter::ConvertBoneAxis(
        ReferenceSkeleton,
        ConvertedBones))
    {
        return false;
    }

    // ---------------------------------------
    // 4. Hand / Finger Twist Correction
    // ---------------------------------------

    if (!FSkeletonConverter::CorrectBoneTwist(
        ReferenceSkeleton,
        ConvertedBones))
    {
        return false;
    }

    // ---------------------------------------
    // 5. Skeleton Hierarchy 검증
    // ---------------------------------------

    if (!FSkeletonConverter::ValidateHierarchy(
        ConvertedBones))
    {
        return false;
    }

    // ---------------------------------------
    // 6. Skeleton C 에셋 생성
    // ---------------------------------------

    TStrongObjectPtr<USkeleton> TargetSkeleton(
        FSkeletonConverter::CreateSkeletonAsset(
            ConvertedBones,
            OutputFolder,
            SkeletonAssetName
        )
    );

    if (!IsValid(TargetSkeleton.Get()))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Create Target Skeleton Failed"));

        return false;
    }

    // ---------------------------------------
    // 7. Preview SkeletalMesh 생성
    // ---------------------------------------

    TStrongObjectPtr<USkeletalMesh> PreviewMesh(
        FSkeletalMeshConverter::CreatePreviewMesh(
            TargetSkeleton.Get(),
            OutputFolder,
            MeshAssetName
        )
    );

    if (!IsValid(PreviewMesh.Get()))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Create Preview Mesh Failed"));

        return false;
    }

    // ---------------------------------------
    // 8. Preview Mesh가 설정된 Skeleton 저장
    // ---------------------------------------

    UPackage* SkeletonPackage =
        TargetSkeleton->GetOutermost();

    const FString SkeletonFileName =
        FPackageName::LongPackageNameToFilename(
            SkeletonPackage->GetName(),
            FPackageName::GetAssetPackageExtension()
        );

    FSavePackageArgs SaveArgs;

    SaveArgs.TopLevelFlags =
        RF_Public | RF_Standalone;

    SaveArgs.SaveFlags = SAVE_None;

    if (!UPackage::SavePackage(
        SkeletonPackage,
        TargetSkeleton.Get(),
        *SkeletonFileName,
        SaveArgs))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Save Target Skeleton Failed"));

        return false;
    }

    // ---------------------------------------
    // 9. 관련 Animation 전체 변환
    // ---------------------------------------

    FCharacterConverterResult Result;

    const bool bSuccess =
        ConvertAllAnimations(
            SourceSkeleton,
            TargetSkeleton.Get(),
            OutputFolder,
            Result
        );

    // ---------------------------------------
    // 10. 최종 결과
    // ---------------------------------------

    const double TotalElapsedTime =
        FPlatformTime::Seconds() -
        StartTime;

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Character Conversion Finished"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Target Skeleton : %s"),
        *TargetSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Preview Mesh : %s"),
        *PreviewMesh->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Total Elapsed Time : %.3f Seconds"),
        TotalElapsedTime);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    return bSuccess;
}

void FCharacterConverter::DumpResult(
    const FCharacterConverterResult& Result)
{
    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("BATCH CONVERSION REPORT"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Total   : %d"),
        Result.TotalCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Success : %d"),
        Result.SuccessCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Failed  : %d"),
        Result.FailedCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Skipped : %d"),
        Result.SkippedCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Elapsed : %.3f Seconds"),
        Result.ElapsedTime);

    for (const FString& Failed :
         Result.FailedAssets)
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("[FAILED] %s"),
            *Failed);
    }

    for (const FString& Skipped :
         Result.SkippedAssets)
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("[SKIPPED] %s"),
            *Skipped);
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));
}

void FCharacterConverter::CommandConvert(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("Usage: CharacterConverter.Convert ")
            TEXT("SourceSkeleton ReferenceSkeleton OutputFolder"));

        return;
    }

    USkeleton* SourceSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    USkeleton* ReferenceSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(SourceSkeleton) ||
        !IsValid(ReferenceSkeleton))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Skeleton Load Failed"));

        return;
    }

    if (!ConvertCharacter(
        SourceSkeleton,
        ReferenceSkeleton,
        Args[2]))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Character Conversion Failed or Partial"));
    }
}

void FCharacterConverter::CommandConvertAnimations(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("Usage: CharacterConverter.Animations ")
            TEXT("SourceSkeleton TargetSkeleton OutputFolder"));

        return;
    }

    USkeleton* SourceSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    USkeleton* TargetSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(SourceSkeleton) ||
        !IsValid(TargetSkeleton))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Skeleton Load Failed"));

        return;
    }

    FCharacterConverterResult Result;

    ConvertAllAnimations(
        SourceSkeleton,
        TargetSkeleton,
        Args[2],
        Result
    );
}

void FCharacterConverter::CommandListAnimations(
    const TArray<FString>& Args)
{
    if (Args.Num() != 1)
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("Usage: CharacterConverter.List SourceSkeleton"));

        return;
    }

    USkeleton* SourceSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    if (!IsValid(SourceSkeleton))
        return;

    TArray<FAssetData> Assets;

    if (!FindAnimations(
        SourceSkeleton,
        Assets))
    {
        return;
    }

    for (int32 i = 0;
         i < Assets.Num();
         ++i)
    {
        UE_LOG(LogCharacterConverter, Log,
            TEXT("[%d] %s"),
            i,
            *Assets[i]
                .GetSoftObjectPath()
                .ToString());
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Total AnimSequences : %d"),
        Assets.Num());
}

bool FCharacterConverter::ConvertAll(
    USkeleton* SourceSkeleton,
    USkeleton* TargetSkeleton,
    const FString& OutputFolder)
{
    if (!IsInGameThread() ||
        !IsValid(SourceSkeleton) ||
        !IsValid(TargetSkeleton) ||
        SourceSkeleton == TargetSkeleton)
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Invalid Conversion Parameters"));

        return false;
    }

    const double StartTime =
        FPlatformTime::Seconds();

    // ---------------------------------------
    // 1. Output Asset Name
    // ---------------------------------------

    FString BaseName =
        SourceSkeleton->GetName();

    BaseName.RemoveFromEnd(TEXT("_Skeleton"));

    const FString SkeletonAssetName =
        BaseName + TEXT("_Converted_Skeleton");

    const FString MeshAssetName =
        BaseName + TEXT("_Converted");

    const FString SkeletonPackageName =
        OutputFolder / SkeletonAssetName;

    const FString MeshPackageName =
        OutputFolder / MeshAssetName;

    // Output Package 검증

    if (!FPackageName::IsValidLongPackageName(
            SkeletonPackageName) ||
        !FPackageName::IsValidLongPackageName(
            MeshPackageName))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Invalid Output Folder : %s"),
            *OutputFolder);

        return false;
    }

    // 기존 에셋 덮어쓰기 방지

    if (FindPackage(nullptr, *SkeletonPackageName) ||
        FindPackage(nullptr, *MeshPackageName) ||
        FPackageName::DoesPackageExist(
            SkeletonPackageName) ||
        FPackageName::DoesPackageExist(
            MeshPackageName))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Output Asset Already Exists"));

        return false;
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Convert All Start"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Source Skeleton : %s"),
        *SourceSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Target Skeleton : %s"),
        *TargetSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Output Folder : %s"),
        *OutputFolder);

    // =======================================
    // STEP 1. Convert_Skeleton
    // =======================================

    UE_LOG(LogCharacterConverter, Log,
        TEXT("[STEP 1/3] Convert_Skeleton"));

    TStrongObjectPtr<USkeleton> ConvertedSkeleton(
        FSkeletonConverter::Convert(
            SourceSkeleton,
            TargetSkeleton,
            OutputFolder,
            SkeletonAssetName
        )
    );

    if (!IsValid(ConvertedSkeleton.Get()))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Convert_Skeleton Failed"));

        return false;
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Convert_Skeleton Success"));

    // =======================================
    // STEP 2. Convert_SkeletalMesh
    // =======================================

    UE_LOG(LogCharacterConverter, Log,
        TEXT("[STEP 2/3] Convert_SkeletalMesh"));

    TStrongObjectPtr<USkeletalMesh> ConvertedMesh(
        FSkeletalMeshConverter::CreatePreviewMesh(
            ConvertedSkeleton.Get(),
            OutputFolder,
            MeshAssetName
        )
    );

    if (!IsValid(ConvertedMesh.Get()))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Convert_SkeletalMesh Failed"));

        return false;
    }

    // Preview Mesh가 설정된 Skeleton 저장

    UPackage* SkeletonPackage =
        ConvertedSkeleton->GetOutermost();

    const FString SkeletonFileName =
        FPackageName::LongPackageNameToFilename(
            SkeletonPackage->GetName(),
            FPackageName::GetAssetPackageExtension()
        );

    FSavePackageArgs SaveArgs;

    SaveArgs.TopLevelFlags =
        RF_Public | RF_Standalone;

    SaveArgs.SaveFlags = SAVE_None;

    if (!UPackage::SavePackage(
        SkeletonPackage,
        ConvertedSkeleton.Get(),
        *SkeletonFileName,
        SaveArgs))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Save Converted Skeleton Failed"));

        return false;
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Convert_SkeletalMesh Success"));

    // =======================================
    // STEP 3. Convert_Animations
    // =======================================

    UE_LOG(LogCharacterConverter, Log,
        TEXT("[STEP 3/3] Convert_Animations"));

    FCharacterConverterResult Result;

    const bool bAnimationSuccess =
        ConvertAllAnimations(
            SourceSkeleton,
            ConvertedSkeleton.Get(),
            OutputFolder,
            Result
        );

    // =======================================
    // Final Result
    // =======================================

    const double ElapsedTime =
        FPlatformTime::Seconds() - StartTime;

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("CONVERT ALL REPORT"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Skeleton : SUCCESS"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("SkeletalMesh : SUCCESS"));

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Animations : %d / %d"),
        Result.SuccessCount,
        Result.TotalCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Failed Animations : %d"),
        Result.FailedCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Skipped Animations : %d"),
        Result.SkippedCount);

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Output Skeleton : %s"),
        *ConvertedSkeleton->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Output SkeletalMesh : %s"),
        *ConvertedMesh->GetPathName());

    UE_LOG(LogCharacterConverter, Log,
        TEXT("Total Time : %.3f Seconds"),
        ElapsedTime);

    if (bAnimationSuccess &&
        Result.SkippedCount == 0)
    {
        UE_LOG(LogCharacterConverter, Log,
            TEXT("Convert All Success"));
    }
    else
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("Convert All Partial or Failed"));
    }

    UE_LOG(LogCharacterConverter, Log,
        TEXT("================================"));

    return bAnimationSuccess &&
           Result.SkippedCount == 0;
}

void FCharacterConverter::CommandConvertAll(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogCharacterConverter, Warning,
            TEXT("Usage: CharacterConverter.ConvertAll ")
            TEXT("SourceSkeleton TargetSkeleton OutputFolder"));

        return;
    }

    USkeleton* SourceSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    USkeleton* TargetSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    const FString& OutputFolder =
        Args[2];

    if (!IsValid(SourceSkeleton))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Source Skeleton Load Failed : %s"),
            *Args[0]);

        return;
    }

    if (!IsValid(TargetSkeleton))
    {
        UE_LOG(LogCharacterConverter, Error,
            TEXT("Target Skeleton Load Failed : %s"),
            *Args[1]);

        return;
    }

    ConvertAll(
        SourceSkeleton,
        TargetSkeleton,
        OutputFolder
    );
}