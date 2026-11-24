#include "AnimationConverter.h"

#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/IAnimationDataModel.h"

#include "Editor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "Animation/AnimData/IAnimationDataController.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

DEFINE_LOG_CATEGORY_STATIC(
	LogAnimationConverter,
	Log,
	All
);

static FAutoConsoleCommand
    GAnimationConverterTestCommand(
        TEXT("AnimationConverter.Test"),
        TEXT("Extract Source Animation Data"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FAnimationConverter::Test
        )
    );

static FAutoConsoleCommand
    GAnimationConverterDrawCommand(
        TEXT("AnimationConverter.Draw"),
        TEXT("Draw Source Animation Pose"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FAnimationConverter::TestDrawPose
        )
    );

static FAutoConsoleCommand
    GAnimationConverterConvertCommand(
        TEXT("AnimationConverter.Convert"),
        TEXT("Convert Source Animation Data"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FAnimationConverter::TestConvert
        )
    );

static FAutoConsoleCommand
    GAnimationConverterDrawConvertedCommand(
        TEXT("AnimationConverter.DrawConverted"),
        TEXT("Draw Source and Converted Animation Pose"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FAnimationConverter::TestDrawConvertedPose
        )
    );

static FAutoConsoleCommand
    GAnimationConverterCreateCommand(
        TEXT("AnimationConverter.Create"),
        TEXT("Create Converted AnimSequence Asset"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FAnimationConverter::TestCreate
        )
    );

bool FAnimationConverter::ExtractAnimationData(
    const UAnimSequence* Animation,
    FAnimationConverterData& OutData)
{
    if (!IsValid(Animation))
        return false;

    const USkeleton* Skeleton =
        Animation->GetSkeleton();

    if (!IsValid(Skeleton))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Source Skeleton Not Found"));

        return false;
    }

    // Additive Animation은 이번 테스트에서 제외

    if (Animation->IsValidAdditive())
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Additive Animation Not Supported"));

        return false;
    }

    // 1. Animation Data Model

    const IAnimationDataModel* DataModel =
        Animation->GetDataModel();

    if (!DataModel)
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation Data Model Not Found"));

        return false;
    }

    FAnimationConverterData NewData;

    NewData.AnimationName =
        Animation->GetFName();

    NewData.AssetPath =
        Animation->GetPathName();

    NewData.FrameRate =
        DataModel->GetFrameRate();

    NewData.NumFrames =
        DataModel->GetNumberOfFrames();

    NewData.NumKeys =
        DataModel->GetNumberOfKeys();

    NewData.PlayLength =
        DataModel->GetPlayLength();

    NewData.bEnableRootMotion =
        Animation->bEnableRootMotion;

    if (NewData.NumKeys <= 0 ||
        NewData.FrameRate.Numerator <= 0 ||
        NewData.FrameRate.Denominator <= 0)
    {
        return false;
    }

    // 2. 원본 Skeleton Reference Pose

    if (!FSkeletonConverter::GetBoneData(
        Skeleton,
        NewData.ReferenceBones))
    {
        return false;
    }

    // 3. Bone Name -> Bone Index

    TMap<FName, int32> BoneIndexMap;

    for (const FSkeletonConverterBone& Bone :
         NewData.ReferenceBones)
    {
        BoneIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex
        );
    }

    NewData.BoneToTrackIndex.Init(
        INDEX_NONE,
        NewData.ReferenceBones.Num()
    );

    // 4. Animation Track 목록 추출

    TArray<FName> TrackNames;

    DataModel->GetBoneTrackNames(
        TrackNames
    );

    // 5. Track별 Transform 추출

    for (const FName& TrackName : TrackNames)
    {
        const int32* BoneIndex =
            BoneIndexMap.Find(TrackName);

        if (!BoneIndex)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Animation Track Bone Not Found : %s"),
                *TrackName.ToString());

            return false;
        }

        if (NewData.BoneToTrackIndex[*BoneIndex]
            != INDEX_NONE)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Duplicate Animation Track : %s"),
                *TrackName.ToString());

            return false;
        }

        const int32 TrackIndex =
            NewData.Tracks.AddDefaulted();

        FAnimationConverterTrack& Track =
            NewData.Tracks[TrackIndex];

        Track.BoneName = TrackName;
        Track.BoneIndex = *BoneIndex;

        Track.Keys.Reserve(NewData.NumKeys);

        // 6. 전체 프레임 샘플링

        for (int32 FrameIndex = 0;
             FrameIndex < NewData.NumKeys;
             ++FrameIndex)
        {
            FTransform Transform =
                DataModel->GetBoneTrackTransform(
                    TrackName,
                    FFrameNumber(FrameIndex)
                );

            if (Transform.ContainsNaN())
            {
                UE_LOG(LogAnimationConverter, Error,
                    TEXT("Invalid Transform : %s / Frame %d"),
                    *TrackName.ToString(),
                    FrameIndex);

                return false;
            }

            Transform.NormalizeRotation();

            // Quaternion 부호 연속성 유지

            if (!Track.Keys.IsEmpty())
            {
                const FQuat PreviousRotation =
                    Track.Keys.Last().GetRotation();

                FQuat CurrentRotation =
                    Transform.GetRotation();

                if ((PreviousRotation | CurrentRotation)
                    < 0.0)
                {
                    CurrentRotation *= -1.0;

                    Transform.SetRotation(
                        CurrentRotation
                    );
                }
            }

            Track.Keys.Add(Transform);
        }

        NewData.BoneToTrackIndex[*BoneIndex] =
            TrackIndex;
    }

    OutData = MoveTemp(NewData);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Extract Animation Success : %s"),
        *Animation->GetName());

    return true;
}

bool FAnimationConverter::EvaluateSourcePose(
    const FAnimationConverterData& Data,
    int32 FrameIndex,
    TArray<FSkeletonConverterBone>& OutBones)
{
    if (FrameIndex < 0 ||
        FrameIndex >= Data.NumKeys)
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Invalid Frame Index : %d"),
            FrameIndex);

        return false;
    }

    if (Data.ReferenceBones.IsEmpty() ||
        Data.BoneToTrackIndex.Num() !=
        Data.ReferenceBones.Num())
    {
        return false;
    }

    // 1. Reference Pose 복사

    TArray<FSkeletonConverterBone> NewBones =
        Data.ReferenceBones;

    // 2. Animation Local Transform 적용

    for (int32 BoneIndex = 0;
         BoneIndex < NewBones.Num();
         ++BoneIndex)
    {
        FSkeletonConverterBone& Bone =
            NewBones[BoneIndex];

        const int32 TrackIndex =
            Data.BoneToTrackIndex[BoneIndex];

        // Track이 없다면 Reference Pose 유지

        if (TrackIndex == INDEX_NONE)
            continue;

        if (!Data.Tracks.IsValidIndex(TrackIndex))
            return false;

        const FAnimationConverterTrack& Track =
            Data.Tracks[TrackIndex];

        if (!Track.Keys.IsValidIndex(FrameIndex))
            return false;

        Bone.LocalTransform =
            Track.Keys[FrameIndex];
    }

    // 3. Component Pose 재구성

    for (int32 BoneIndex = 0;
         BoneIndex < NewBones.Num();
         ++BoneIndex)
    {
        FSkeletonConverterBone& Bone =
            NewBones[BoneIndex];

        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.ComponentTransform =
                Bone.LocalTransform;
        }
        else
        {
            if (Bone.ParentIndex < 0 ||
                Bone.ParentIndex >= BoneIndex)
            {
                return false;
            }

            const FSkeletonConverterBone& Parent =
                NewBones[Bone.ParentIndex];

            Bone.ComponentTransform =
                Bone.LocalTransform *
                Parent.ComponentTransform;
        }

        Bone.ComponentTransform.NormalizeRotation();

        if (Bone.ComponentTransform.ContainsNaN())
            return false;
    }

    OutBones = MoveTemp(NewBones);

    return true;
}

void FAnimationConverter::DumpAnimationData(
    const FAnimationConverterData& Data)
{
    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation : %s"),
        *Data.AnimationName.ToString());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Frame Rate : %d / %d (%.4f FPS)"),
        Data.FrameRate.Numerator,
        Data.FrameRate.Denominator,
        Data.FrameRate.AsDecimal());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Num Frames : %d"),
        Data.NumFrames);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Num Keys : %d"),
        Data.NumKeys);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Play Length : %.6f Seconds"),
        Data.PlayLength);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Reference Bones : %d"),
        Data.ReferenceBones.Num());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation Tracks : %d"),
        Data.Tracks.Num());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Root Motion : %s"),
        Data.bEnableRootMotion
            ? TEXT("Enabled")
            : TEXT("Disabled"));

    int32 MissingTrackCount = 0;

    for (int32 TrackIndex :
         Data.BoneToTrackIndex)
    {
        if (TrackIndex == INDEX_NONE)
            ++MissingTrackCount;
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Bones Without Track : %d"),
        MissingTrackCount);

    for (const FAnimationConverterTrack& Track :
         Data.Tracks)
    {
        UE_LOG(LogAnimationConverter, Log,
            TEXT("[Track] %s / BoneIndex=%d / Keys=%d"),
            *Track.BoneName.ToString(),
            Track.BoneIndex,
            Track.Keys.Num());
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));
}

static TSharedRef<FJsonObject>
AnimationTransformToJson(
    const FTransform& Transform)
{
    TSharedRef<FJsonObject> Object =
        MakeShared<FJsonObject>();

    const FVector Location =
        Transform.GetLocation();

    const FQuat Rotation =
        Transform.GetRotation();

    const FVector Scale =
        Transform.GetScale3D();

    TArray<TSharedPtr<FJsonValue>> LocationArray;
    TArray<TSharedPtr<FJsonValue>> RotationArray;
    TArray<TSharedPtr<FJsonValue>> ScaleArray;

    LocationArray.Add(
        MakeShared<FJsonValueNumber>(Location.X));
    LocationArray.Add(
        MakeShared<FJsonValueNumber>(Location.Y));
    LocationArray.Add(
        MakeShared<FJsonValueNumber>(Location.Z));

    RotationArray.Add(
        MakeShared<FJsonValueNumber>(Rotation.X));
    RotationArray.Add(
        MakeShared<FJsonValueNumber>(Rotation.Y));
    RotationArray.Add(
        MakeShared<FJsonValueNumber>(Rotation.Z));
    RotationArray.Add(
        MakeShared<FJsonValueNumber>(Rotation.W));

    ScaleArray.Add(
        MakeShared<FJsonValueNumber>(Scale.X));
    ScaleArray.Add(
        MakeShared<FJsonValueNumber>(Scale.Y));
    ScaleArray.Add(
        MakeShared<FJsonValueNumber>(Scale.Z));

    Object->SetArrayField(
        TEXT("Location"), LocationArray);

    Object->SetArrayField(
        TEXT("Rotation"), RotationArray);

    Object->SetArrayField(
        TEXT("Scale"), ScaleArray);

    return Object;
}

bool FAnimationConverter::ExportJson(
    const FAnimationConverterData& Data,
    const FString& FilePath)
{
    if (Data.NumKeys <= 0)
        return false;

    TSharedRef<FJsonObject> Root =
        MakeShared<FJsonObject>();

    Root->SetStringField(
        TEXT("AnimationName"),
        Data.AnimationName.ToString());

    Root->SetStringField(
        TEXT("AssetPath"),
        Data.AssetPath);

    Root->SetNumberField(
        TEXT("FrameRateNumerator"),
        Data.FrameRate.Numerator);

    Root->SetNumberField(
        TEXT("FrameRateDenominator"),
        Data.FrameRate.Denominator);

    Root->SetNumberField(
        TEXT("FrameRate"),
        Data.FrameRate.AsDecimal());

    Root->SetNumberField(
        TEXT("NumFrames"),
        Data.NumFrames);

    Root->SetNumberField(
        TEXT("NumKeys"),
        Data.NumKeys);

    Root->SetNumberField(
        TEXT("PlayLength"),
        Data.PlayLength);

    Root->SetNumberField(
        TEXT("BoneCount"),
        Data.ReferenceBones.Num());

    Root->SetNumberField(
        TEXT("TrackCount"),
        Data.Tracks.Num());

    Root->SetBoolField(
        TEXT("RootMotion"),
        Data.bEnableRootMotion);

    // -----------------------------------
    // 1. Animation Track 정보
    // -----------------------------------

    TArray<TSharedPtr<FJsonValue>> TrackArray;

    for (const FAnimationConverterTrack& Track :
         Data.Tracks)
    {
        if (Track.Keys.IsEmpty())
            return false;

        TSharedRef<FJsonObject> Object =
            MakeShared<FJsonObject>();

        Object->SetStringField(
            TEXT("BoneName"),
            Track.BoneName.ToString());

        Object->SetNumberField(
            TEXT("BoneIndex"),
            Track.BoneIndex);

        Object->SetNumberField(
            TEXT("KeyCount"),
            Track.Keys.Num());

        bool bLocationAnimated = false;
        bool bRotationAnimated = false;
        bool bScaleAnimated = false;

        const FTransform& First =
            Track.Keys[0];

        for (const FTransform& Key : Track.Keys)
        {
            if (!First.GetLocation().Equals(
                Key.GetLocation(), 0.0001))
            {
                bLocationAnimated = true;
            }

            if (First.GetRotation().AngularDistance(
                Key.GetRotation()) > 0.00001)
            {
                bRotationAnimated = true;
            }

            if (!First.GetScale3D().Equals(
                Key.GetScale3D(), 0.0001))
            {
                bScaleAnimated = true;
            }
        }

        Object->SetBoolField(
            TEXT("LocationAnimated"),
            bLocationAnimated);

        Object->SetBoolField(
            TEXT("RotationAnimated"),
            bRotationAnimated);

        Object->SetBoolField(
            TEXT("ScaleAnimated"),
            bScaleAnimated);

        Object->SetObjectField(
            TEXT("FirstTransform"),
            AnimationTransformToJson(
                Track.Keys[0]));

        Object->SetObjectField(
            TEXT("LastTransform"),
            AnimationTransformToJson(
                Track.Keys.Last()));

        TrackArray.Add(
            MakeShared<FJsonValueObject>(Object)
        );
    }

    Root->SetArrayField(
        TEXT("Tracks"),
        TrackArray);

    // -----------------------------------
    // 2. Sample Pose
    // -----------------------------------

    TArray<int32> SampleFrames;

    SampleFrames.AddUnique(0);

    SampleFrames.AddUnique(
        Data.NumKeys / 2);

    SampleFrames.AddUnique(
        Data.NumKeys - 1);

    TArray<TSharedPtr<FJsonValue>> PoseArray;

    for (const int32 FrameIndex : SampleFrames)
    {
        TArray<FSkeletonConverterBone> Bones;

        if (!EvaluateSourcePose(
            Data,
            FrameIndex,
            Bones))
        {
            return false;
        }

        TSharedRef<FJsonObject> PoseObject =
            MakeShared<FJsonObject>();

        PoseObject->SetNumberField(
            TEXT("FrameIndex"),
            FrameIndex);

        PoseObject->SetNumberField(
            TEXT("Time"),
            static_cast<double>(FrameIndex) /
            Data.FrameRate.AsDecimal());

        TArray<TSharedPtr<FJsonValue>> BoneArray;

        for (const FSkeletonConverterBone& Bone :
             Bones)
        {
            TSharedRef<FJsonObject> BoneObject =
                MakeShared<FJsonObject>();

            BoneObject->SetStringField(
                TEXT("BoneName"),
                Bone.BoneName.ToString());

            BoneObject->SetNumberField(
                TEXT("BoneIndex"),
                Bone.BoneIndex);

            BoneObject->SetNumberField(
                TEXT("ParentIndex"),
                Bone.ParentIndex);

            BoneObject->SetObjectField(
                TEXT("LocalTransform"),
                AnimationTransformToJson(
                    Bone.LocalTransform));

            BoneObject->SetObjectField(
                TEXT("ComponentTransform"),
                AnimationTransformToJson(
                    Bone.ComponentTransform));

            BoneArray.Add(
                MakeShared<FJsonValueObject>(
                    BoneObject));
        }

        PoseObject->SetArrayField(
            TEXT("Bones"),
            BoneArray);

        PoseArray.Add(
            MakeShared<FJsonValueObject>(
                PoseObject));
    }

    Root->SetArrayField(
        TEXT("SamplePoses"),
        PoseArray);

    // -----------------------------------
    // 3. JSON 저장
    // -----------------------------------

    FString OutputString;

    TSharedRef<TJsonWriter<>> Writer =
        TJsonWriterFactory<>::Create(
            &OutputString
        );

    if (!FJsonSerializer::Serialize(
        Root, Writer))
    {
        return false;
    }

    IFileManager::Get().MakeDirectory(
        *FPaths::GetPath(FilePath),
        true
    );

    return FFileHelper::SaveStringToFile(
        OutputString,
        *FilePath,
        FFileHelper::EEncodingOptions::ForceUTF8
    );
}

void FAnimationConverter::TestDrawPose(
    const TArray<FString>& Args)
{
    if (Args.Num() != 2)
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Usage: AnimationConverter.Draw AssetPath Frame"));

        return;
    }

    if (!GEditor)
        return;

    UWorld* World =
        GEditor->GetEditorWorldContext().World();

    if (!IsValid(World))
        return;

    UAnimSequence* Animation =
        LoadObject<UAnimSequence>(
            nullptr,
            *Args[0]
        );

    if (!IsValid(Animation))
        return;

    int32 FrameIndex = 0;

    if (!LexTryParseString(
        FrameIndex,
        *Args[1]))
    {
        return;
    }

    FAnimationConverterData Data;

    if (!ExtractAnimationData(
        Animation,
        Data))
    {
        return;
    }

    TArray<FSkeletonConverterBone> PoseBones;

    if (!EvaluateSourcePose(
        Data,
        FrameIndex,
        PoseBones))
    {
        return;
    }

    const USkeleton* Skeleton =
        Animation->GetSkeleton();

    FlushPersistentDebugLines(World);
    FlushDebugStrings(World);

    constexpr float Spacing = 150.0f;

    // Reference Pose

    FSkeletonConverter::DrawDebugSkeletonBones(
        World,
        Skeleton,
        Data.ReferenceBones,
        FTransform(
            FQuat::Identity,
            FVector(-Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        5.0f,
        -1.0f,
        FColor::White
    );

    // Animation Pose

    FSkeletonConverter::DrawDebugSkeletonBones(
        World,
        Skeleton,
        PoseBones,
        FTransform(
            FQuat::Identity,
            FVector(Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        5.0f,
        -1.0f,
        FColor::Yellow
    );

    GEditor->RedrawLevelEditingViewports();

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation Pose Draw : %s / Frame %d / %d"),
        *Animation->GetName(),
        FrameIndex,
        Data.NumKeys - 1);
}

bool FAnimationConverter::BuildAnimationMapping(
    const FAnimationConverterData& SourceData,
    const USkeleton* TargetSkeleton,
    FAnimationConverterMapping& OutMapping)
{
    if (!IsValid(TargetSkeleton) ||
        SourceData.ReferenceBones.IsEmpty())
    {
        return false;
    }

    FAnimationConverterMapping NewMapping;

    // 1. Target Reference Pose 추출

    if (!FSkeletonConverter::GetBoneData(
        TargetSkeleton,
        NewMapping.TargetReferenceBones))
    {
        return false;
    }

    // 2. Source Bone Index Map

    TMap<FName, int32> SourceIndexMap;

    for (const FSkeletonConverterBone& Bone :
         SourceData.ReferenceBones)
    {
        SourceIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex
        );
    }

    // 3. 기존 Skeleton Mapping 사용

    const TConstArrayView<FSkeletonConverterMapping>
        BoneMapping =
            FSkeletonConverter::GetBoneMapping();

    TMap<FName, FName> TargetToSourceName;

    for (const FSkeletonConverterMapping& Entry :
         BoneMapping)
    {
        if (TargetToSourceName.Contains(
            Entry.TargetBone))
        {
            return false;
        }

        TargetToSourceName.Add(
            Entry.TargetBone,
            Entry.SourceBone
        );
    }

    if (NewMapping.TargetReferenceBones.Num() !=
        BoneMapping.Num())
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Target Bone Count Mismatch"));

        return false;
    }

    NewMapping.BoneMappings.SetNum(
        NewMapping.TargetReferenceBones.Num()
    );

    // 4. Target Bone 순서로 Mapping 구성

    for (const FSkeletonConverterBone& TargetBone :
         NewMapping.TargetReferenceBones)
    {
        const FName* SourceBoneName =
            TargetToSourceName.Find(
                TargetBone.BoneName
            );

        if (!SourceBoneName)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Target Mapping Not Found : %s"),
                *TargetBone.BoneName.ToString());

            return false;
        }

        const int32* SourceBoneIndex =
            SourceIndexMap.Find(*SourceBoneName);

        if (!SourceBoneIndex)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Source Bone Not Found : %s"),
                *SourceBoneName->ToString());

            return false;
        }

        const FSkeletonConverterBone& SourceBone =
            SourceData.ReferenceBones[
                *SourceBoneIndex
            ];

        const FQuat SourceRefRotation =
            SourceBone.ComponentTransform.GetRotation();

        const FQuat TargetRefRotation =
            TargetBone.ComponentTransform.GetRotation();

        // 5. Reference Axis Offset

        FQuat AxisOffset =
            SourceRefRotation.Inverse() *
            TargetRefRotation;

        AxisOffset.Normalize();

        if (AxisOffset.ContainsNaN())
            return false;

        // A와 C의 Reference 위치가 동일한지 확인
        // 이 변환은 동일한 관절 위치를 전제로 함

        const double PositionError =
            FVector::Distance(
                SourceBone.ComponentTransform
                    .GetLocation(),

                TargetBone.ComponentTransform
                    .GetLocation()
            );

        if (PositionError > 0.001)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Reference Position Mismatch : %s"),
                *TargetBone.BoneName.ToString());

            return false;
        }

        if (!SourceBone.ComponentTransform.GetScale3D()
            .Equals(
                TargetBone.ComponentTransform.GetScale3D(),
                0.0001))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Reference Scale Mismatch : %s"),
                *TargetBone.BoneName.ToString());

            return false;
        }

        FAnimationConverterBoneMapping& Entry =
            NewMapping.BoneMappings[
                TargetBone.BoneIndex
            ];

        Entry.SourceBoneIndex =
            SourceBone.BoneIndex;

        Entry.TargetBoneIndex =
            TargetBone.BoneIndex;

        Entry.SourceBoneName =
            SourceBone.BoneName;

        Entry.TargetBoneName =
            TargetBone.BoneName;

        Entry.AxisOffset = AxisOffset;

        const double OffsetAngle =
            FMath::RadiansToDegrees(
                FQuat::Identity.AngularDistance(
                    AxisOffset
                )
            );

        UE_LOG(LogAnimationConverter, Log,
            TEXT("[Mapping] %s -> %s / Offset=%.4f"),
            *Entry.SourceBoneName.ToString(),
            *Entry.TargetBoneName.ToString(),
            OffsetAngle);
    }

    OutMapping = MoveTemp(NewMapping);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Build Animation Mapping Success : %d Bones"),
        OutMapping.BoneMappings.Num());

    return true;
}


bool FAnimationConverter::ConvertAnimationPose(
    const TArray<FSkeletonConverterBone>& SourcePose,
    const FAnimationConverterMapping& Mapping,
    TArray<FSkeletonConverterBone>& OutTargetPose)
{
    if (SourcePose.IsEmpty() ||
        Mapping.TargetReferenceBones.IsEmpty() ||
        Mapping.BoneMappings.Num() !=
        Mapping.TargetReferenceBones.Num())
    {
        return false;
    }

    TArray<FSkeletonConverterBone> TargetPose =
        Mapping.TargetReferenceBones;

    // ---------------------------------------
    // 1. Target Component Transform 계산
    // ---------------------------------------

    for (int32 TargetIndex = 0;
         TargetIndex < TargetPose.Num();
         ++TargetIndex)
    {
        const FAnimationConverterBoneMapping& Entry =
            Mapping.BoneMappings[TargetIndex];

        if (!SourcePose.IsValidIndex(
            Entry.SourceBoneIndex))
        {
            return false;
        }

        FSkeletonConverterBone& TargetBone =
            TargetPose[TargetIndex];

        const FSkeletonConverterBone& SourceBone =
            SourcePose[Entry.SourceBoneIndex];

        if (Entry.TargetBoneIndex != TargetIndex ||
            TargetBone.BoneName != Entry.TargetBoneName ||
            SourceBone.BoneName != Entry.SourceBoneName)
        {
            return false;
        }

        const FTransform& SourceComponent =
            SourceBone.ComponentTransform;

        // Location 유지

        const FVector NewLocation =
            SourceComponent.GetLocation();

        // Rotation Axis 변환

        FQuat NewRotation =
            SourceComponent.GetRotation() *
            Entry.AxisOffset;

        NewRotation.Normalize();

        // Scale 유지

        const FVector NewScale =
            SourceComponent.GetScale3D();

        TargetBone.ComponentTransform =
            FTransform(
                NewRotation,
                NewLocation,
                NewScale
            );

        if (TargetBone.ComponentTransform.ContainsNaN())
        {
            return false;
        }
    }

    // ---------------------------------------
    // 2. Target Local Transform 재구성
    // ---------------------------------------

    for (int32 BoneIndex = 0;
         BoneIndex < TargetPose.Num();
         ++BoneIndex)
    {
        FSkeletonConverterBone& Bone =
            TargetPose[BoneIndex];

        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.LocalTransform =
                Bone.ComponentTransform;
        }
        else
        {
            if (Bone.ParentIndex < 0 ||
                Bone.ParentIndex >= BoneIndex)
            {
                return false;
            }

            const FSkeletonConverterBone& Parent =
                TargetPose[Bone.ParentIndex];

            const FVector ParentScale =
                Parent.ComponentTransform.GetScale3D();

            // 역변환이 불가능한 Scale 차단

            if (FMath::IsNearlyZero(ParentScale.X) ||
                FMath::IsNearlyZero(ParentScale.Y) ||
                FMath::IsNearlyZero(ParentScale.Z))
            {
                UE_LOG(LogAnimationConverter, Error,
                    TEXT("Invalid Parent Scale : %s"),
                    *Parent.BoneName.ToString());

                return false;
            }

            Bone.LocalTransform =
                Bone.ComponentTransform
                    .GetRelativeTransform(
                        Parent.ComponentTransform
                    );
        }

        Bone.LocalTransform.NormalizeRotation();

        if (Bone.LocalTransform.ContainsNaN())
            return false;
    }

    // ---------------------------------------
    // 3. 변환 결과 검증
    // ---------------------------------------

    if (!ValidateConvertedPose(
        SourcePose,
        TargetPose,
        Mapping))
    {
        return false;
    }

    OutTargetPose = MoveTemp(TargetPose);

    return true;
}

bool FAnimationConverter::ValidateConvertedPose(
    const TArray<FSkeletonConverterBone>& SourcePose,
    const TArray<FSkeletonConverterBone>& TargetPose,
    const FAnimationConverterMapping& Mapping)
{
    if (TargetPose.Num() !=
        Mapping.BoneMappings.Num())
    {
        return false;
    }

    constexpr double PositionTolerance = 0.001;
    constexpr double RotationTolerance = 0.01;
    constexpr double ScaleTolerance = 0.0001;

    TArray<FTransform> ReconstructedTransforms;

    ReconstructedTransforms.SetNum(
        TargetPose.Num()
    );

    for (int32 i = 0;
         i < TargetPose.Num();
         ++i)
    {
        const FAnimationConverterBoneMapping& Entry =
            Mapping.BoneMappings[i];

        if (!SourcePose.IsValidIndex(
            Entry.SourceBoneIndex))
        {
            return false;
        }

        const FSkeletonConverterBone& SourceBone =
            SourcePose[Entry.SourceBoneIndex];

        const FSkeletonConverterBone& TargetBone =
            TargetPose[i];

        const FTransform& SourceComponent =
            SourceBone.ComponentTransform;

        const FTransform& TargetComponent =
            TargetBone.ComponentTransform;

        // 1. 원본 Animation 위치 보존 검사

        const double PositionError =
            FVector::Distance(
                SourceComponent.GetLocation(),
                TargetComponent.GetLocation()
            );

        // 2. 원본 Rotation 움직임 보존 검사
        // Target에서 AxisOffset을 제거하면
        // Source Rotation과 동일해야 함

        FQuat RecoveredSourceRotation =
            TargetComponent.GetRotation() *
            Entry.AxisOffset.Inverse();

        RecoveredSourceRotation.Normalize();

        const double RotationError =
            FMath::RadiansToDegrees(
                RecoveredSourceRotation.AngularDistance(
                    SourceComponent.GetRotation()
                )
            );

        // 3. Scale 보존 검사

        const double ScaleError =
            FVector::Distance(
                SourceComponent.GetScale3D(),
                TargetComponent.GetScale3D()
            );

        if (PositionError > PositionTolerance ||
            RotationError > RotationTolerance ||
            ScaleError > ScaleTolerance)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Source Pose Preservation Failed : %s")
                TEXT(" / Pos=%.8f Rot=%.8f Scale=%.8f"),
                *TargetBone.BoneName.ToString(),
                PositionError,
                RotationError,
                ScaleError);

            return false;
        }

        // 4. Local -> Component 재구성

        FTransform Reconstructed;

        if (TargetBone.ParentIndex == INDEX_NONE)
        {
            if (i != 0)
                return false;

            Reconstructed =
                TargetBone.LocalTransform;
        }
        else
        {
            if (TargetBone.ParentIndex < 0 ||
                TargetBone.ParentIndex >= i)
            {
                return false;
            }

            Reconstructed =
                TargetBone.LocalTransform *
                ReconstructedTransforms[
                    TargetBone.ParentIndex
                ];
        }

        Reconstructed.NormalizeRotation();

        if (Reconstructed.ContainsNaN())
            return false;

        ReconstructedTransforms[i] =
            Reconstructed;

        const double HierarchyPositionError =
            FVector::Distance(
                Reconstructed.GetLocation(),
                TargetComponent.GetLocation()
            );

        const double HierarchyRotationError =
            FMath::RadiansToDegrees(
                Reconstructed.GetRotation()
                    .AngularDistance(
                        TargetComponent.GetRotation()
                    )
            );

        const double HierarchyScaleError =
            FVector::Distance(
                Reconstructed.GetScale3D(),
                TargetComponent.GetScale3D()
            );

        if (HierarchyPositionError >
                PositionTolerance ||

            HierarchyRotationError >
                RotationTolerance ||

            HierarchyScaleError >
                ScaleTolerance)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Target Hierarchy Validation Failed : %s")
                TEXT(" / Pos=%.8f Rot=%.8f Scale=%.8f"),
                *TargetBone.BoneName.ToString(),
                HierarchyPositionError,
                HierarchyRotationError,
                HierarchyScaleError);

            return false;
        }
    }

    return true;
}

bool FAnimationConverter::ConvertAnimationData(
    const FAnimationConverterData& SourceData,
    const USkeleton* TargetSkeleton,
    FAnimationConverterData& OutData)
{
    if (!IsValid(TargetSkeleton) ||
        SourceData.NumKeys <= 0)
    {
        return false;
    }

    // 1. Animation Mapping 생성

    FAnimationConverterMapping Mapping;

    if (!BuildAnimationMapping(
        SourceData,
        TargetSkeleton,
        Mapping))
    {
        return false;
    }

    // Reference Pose 변환 자체도 검증

    TArray<FSkeletonConverterBone> ConvertedReference;

    if (!ConvertAnimationPose(
        SourceData.ReferenceBones,
        Mapping,
        ConvertedReference))
    {
        return false;
    }

    for (int32 i = 0;
         i < ConvertedReference.Num();
         ++i)
    {
        const FTransform& Converted =
            ConvertedReference[i].LocalTransform;

        const FTransform& Expected =
            Mapping.TargetReferenceBones[i]
                .LocalTransform;

        if (!Converted.GetLocation().Equals(
                Expected.GetLocation(), 0.001) ||
            FMath::RadiansToDegrees(
                Converted.GetRotation().AngularDistance(
                    Expected.GetRotation())) > 0.01 ||
            !Converted.GetScale3D().Equals(
                Expected.GetScale3D(), 0.0001))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Reference Pose Validation Failed : %s"),
                *ConvertedReference[i].BoneName.ToString());

            return false;
        }
    }

    // 2. Target Animation Data 초기화

    FAnimationConverterData NewData;

    NewData.AnimationName =
        FName(
            *FString::Printf(
                TEXT("%s_Converted"),
                *SourceData.AnimationName.ToString()
            )
        );

    // 아직 실제 에셋이 생성되지 않았음
    NewData.AssetPath.Reset();

    NewData.FrameRate =
        SourceData.FrameRate;

    NewData.NumFrames =
        SourceData.NumFrames;

    NewData.NumKeys =
        SourceData.NumKeys;

    NewData.PlayLength =
        SourceData.PlayLength;

    NewData.bEnableRootMotion =
        SourceData.bEnableRootMotion;

    NewData.ReferenceBones =
        Mapping.TargetReferenceBones;

    const int32 NumTargetBones =
        NewData.ReferenceBones.Num();

    NewData.Tracks.SetNum(
        NumTargetBones
    );

    NewData.BoneToTrackIndex.SetNum(
        NumTargetBones
    );

    // 3. Target Animation Track 생성

    for (int32 BoneIndex = 0;
         BoneIndex < NumTargetBones;
         ++BoneIndex)
    {
        FAnimationConverterTrack& Track =
            NewData.Tracks[BoneIndex];

        Track.BoneName =
            NewData.ReferenceBones[BoneIndex]
                .BoneName;

        Track.BoneIndex = BoneIndex;

        Track.Keys.Reserve(
            NewData.NumKeys
        );

        NewData.BoneToTrackIndex[BoneIndex] =
            BoneIndex;
    }

    TArray<FSkeletonConverterBone> SourcePose;
    TArray<FSkeletonConverterBone> TargetPose;

    // ---------------------------------------
    // 4. 전체 Animation Frame 변환
    // ---------------------------------------

    for (int32 FrameIndex = 0;
         FrameIndex < SourceData.NumKeys;
         ++FrameIndex)
    {
        // Source Animation 151 Bone 평가

        if (!EvaluateSourcePose(
            SourceData,
            FrameIndex,
            SourcePose))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Source Pose Failed : Frame %d"),
                FrameIndex);

            return false;
        }

        // 151 -> 52 Bone Conversion

        if (!ConvertAnimationPose(
            SourcePose,
            Mapping,
            TargetPose))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Convert Pose Failed : Frame %d"),
                FrameIndex);

            return false;
        }

        // Target Local Transform Key 저장

        for (int32 BoneIndex = 0;
             BoneIndex < NumTargetBones;
             ++BoneIndex)
        {
            FAnimationConverterTrack& Track =
                NewData.Tracks[BoneIndex];

            FTransform Key =
                TargetPose[BoneIndex].LocalTransform;

            // Quaternion 부호 연속성

            if (!Track.Keys.IsEmpty())
            {
                const FQuat PreviousRotation =
                    Track.Keys.Last().GetRotation();

                FQuat CurrentRotation =
                    Key.GetRotation();

                if ((PreviousRotation |
                     CurrentRotation) < 0.0)
                {
                    CurrentRotation *= -1.0;

                    Key.SetRotation(
                        CurrentRotation
                    );
                }
            }

            Track.Keys.Add(Key);
        }
    }

    // 5. Key Count 검증

    for (const FAnimationConverterTrack& Track :
         NewData.Tracks)
    {
        if (Track.Keys.Num() !=
            NewData.NumKeys)
        {
            return false;
        }
    }

    OutData = MoveTemp(NewData);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Convert Animation Data Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Source Tracks : %d"),
        SourceData.Tracks.Num());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Target Tracks : %d"),
        OutData.Tracks.Num());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Converted Frames : %d"),
        OutData.NumKeys);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Total Transform Samples : %lld"),
        static_cast<int64>(OutData.Tracks.Num()) *
        OutData.NumKeys);

    return true;
}

void FAnimationConverter::TestDrawConvertedPose(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Usage: AnimationConverter.DrawConverted ")
            TEXT("AnimationPath TargetSkeletonPath Frame"));

        return;
    }

    if (!GEditor)
        return;

    UWorld* World =
        GEditor->GetEditorWorldContext().World();

    if (!IsValid(World))
        return;

    UAnimSequence* Animation =
        LoadObject<UAnimSequence>(
            nullptr,
            *Args[0]
        );

    USkeleton* TargetSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(Animation) ||
        !IsValid(TargetSkeleton))
    {
        return;
    }

    int32 FrameIndex = 0;

    if (!LexTryParseString(
        FrameIndex,
        *Args[2]))
    {
        return;
    }

    // 1. Source Animation 추출

    FAnimationConverterData SourceData;

    if (!ExtractAnimationData(
        Animation,
        SourceData))
    {
        return;
    }

    // 2. Animation 전체 변환

    FAnimationConverterData TargetData;

    if (!ConvertAnimationData(
        SourceData,
        TargetSkeleton,
        TargetData))
    {
        return;
    }

    // 3. 동일 Frame 평가

    TArray<FSkeletonConverterBone> SourcePose;
    TArray<FSkeletonConverterBone> TargetPose;

    if (!EvaluateSourcePose(
        SourceData,
        FrameIndex,
        SourcePose))
    {
        return;
    }

    if (!EvaluateSourcePose(
        TargetData,
        FrameIndex,
        TargetPose))
    {
        return;
    }

    FlushPersistentDebugLines(World);
    FlushDebugStrings(World);

    constexpr float Spacing = 150.0f;

    // 4. Source Animation A

    FSkeletonConverter::DrawDebugSkeletonBones(
        World,
        Animation->GetSkeleton(),
        SourcePose,
        FTransform(
            FQuat::Identity,
            FVector(-Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        5.0f,
        -1.0f,
        FColor::White
    );

    // 5. Converted Animation C

    FSkeletonConverter::DrawDebugSkeletonBones(
        World,
        TargetSkeleton,
        TargetPose,
        FTransform(
            FQuat::Identity,
            FVector(Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        5.0f,
        -1.0f,
        FColor::Yellow
    );

    GEditor->RedrawLevelEditingViewports();

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Draw Converted Animation Success : %d / %d"),
        FrameIndex,
        TargetData.NumKeys - 1);
}
bool FAnimationConverter::ValidateAnimationAsset(
    const UAnimSequence* Animation,
    const FAnimationConverterData& ExpectedData)
{
    if (!IsValid(Animation))
        return false;

    // 1. 생성된 에셋에서 데이터 재추출

    FAnimationConverterData ActualData;

    if (!ExtractAnimationData(
        Animation,
        ActualData))
    {
        return false;
    }

    // 2. Animation 기본 정보 비교

    if (ActualData.FrameRate != ExpectedData.FrameRate ||
        ActualData.NumFrames != ExpectedData.NumFrames ||
        ActualData.NumKeys != ExpectedData.NumKeys ||
        ActualData.Tracks.Num() != ExpectedData.Tracks.Num() ||
        ActualData.ReferenceBones.Num() !=
            ExpectedData.ReferenceBones.Num() ||
        ActualData.bEnableRootMotion !=
            ExpectedData.bEnableRootMotion ||
        !FMath::IsNearlyEqual(
            ActualData.PlayLength,
            ExpectedData.PlayLength,
            0.000001))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation Metadata Mismatch"));

        return false;
    }

    constexpr double PositionTolerance = 0.001;
    constexpr double RotationTolerance = 0.01;
    constexpr double ScaleTolerance = 0.0001;

    double MaxPositionError = 0.0;
    double MaxRotationError = 0.0;
    double MaxScaleError = 0.0;

    int64 ValidatedSamples = 0;

    // 3. Track Name -> Track Index

    TMap<FName, int32> ActualTrackMap;

    for (int32 i = 0;
         i < ActualData.Tracks.Num();
         ++i)
    {
        ActualTrackMap.Add(
            ActualData.Tracks[i].BoneName,
            i
        );
    }

    // 4. 전체 Transform Key 비교

    for (const FAnimationConverterTrack& ExpectedTrack :
         ExpectedData.Tracks)
    {
        const int32* TrackIndex =
            ActualTrackMap.Find(
                ExpectedTrack.BoneName
            );

        if (!TrackIndex)
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Missing Track : %s"),
                *ExpectedTrack.BoneName.ToString());

            return false;
        }

        const FAnimationConverterTrack& ActualTrack =
            ActualData.Tracks[*TrackIndex];

        if (ActualTrack.Keys.Num() !=
            ExpectedTrack.Keys.Num())
        {
            return false;
        }

        for (int32 FrameIndex = 0;
             FrameIndex < ExpectedTrack.Keys.Num();
             ++FrameIndex)
        {
            const FTransform& Expected =
                ExpectedTrack.Keys[FrameIndex];

            const FTransform& Actual =
                ActualTrack.Keys[FrameIndex];

            const double PositionError =
                FVector::Distance(
                    Expected.GetLocation(),
                    Actual.GetLocation()
                );

            const double RotationError =
                FMath::RadiansToDegrees(
                    Expected.GetRotation().AngularDistance(
                        Actual.GetRotation()
                    )
                );

            const double ScaleError =
                FVector::Distance(
                    Expected.GetScale3D(),
                    Actual.GetScale3D()
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

            if (PositionError > PositionTolerance ||
                RotationError > RotationTolerance ||
                ScaleError > ScaleTolerance)
            {
                UE_LOG(LogAnimationConverter, Error,
                    TEXT("Animation Key Validation Failed : %s")
                    TEXT(" / Frame=%d")
                    TEXT(" / Pos=%.8f Rot=%.8f Scale=%.8f"),
                    *ExpectedTrack.BoneName.ToString(),
                    FrameIndex,
                    PositionError,
                    RotationError,
                    ScaleError);

                return false;
            }

            ++ValidatedSamples;
        }
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Validate Animation Asset Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Validated Samples : %lld"),
        ValidatedSamples);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Max Position Error : %.10f cm"),
        MaxPositionError);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Max Rotation Error : %.10f Degrees"),
        MaxRotationError);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Max Scale Error : %.10f"),
        MaxScaleError);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));

    return true;
}

UAnimSequence* FAnimationConverter::CreateAnimSequenceAsset(
    const FAnimationConverterData& Data,
    USkeleton* TargetSkeleton,
    const FString& PackagePath,
    const FString& AssetName)
{
    if (!IsInGameThread() ||
        !IsValid(TargetSkeleton))
    {
        return nullptr;
    }

    if (Data.NumFrames <= 0 ||
        Data.NumKeys != Data.NumFrames + 1 ||
        Data.Tracks.IsEmpty() ||
        Data.FrameRate.Numerator <= 0 ||
        Data.FrameRate.Denominator <= 0)
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Invalid Animation Data"));

        return nullptr;
    }

    // ---------------------------------------
    // 1. Target Skeleton 검증
    // ---------------------------------------

    const FReferenceSkeleton& RefSkeleton =
        TargetSkeleton->GetReferenceSkeleton();

    if (Data.ReferenceBones.Num() !=
        RefSkeleton.GetRawBoneNum())
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Target Skeleton Bone Count Mismatch"));

        return nullptr;
    }

    if (Data.Tracks.Num() !=
        Data.ReferenceBones.Num())
    {
        return nullptr;
    }

    TSet<FName> TrackNames;

    for (const FAnimationConverterTrack& Track :
         Data.Tracks)
    {
        if (Track.Keys.Num() != Data.NumKeys ||
            RefSkeleton.FindRawBoneIndex(
                Track.BoneName) == INDEX_NONE ||
            TrackNames.Contains(Track.BoneName))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Invalid Track : %s"),
                *Track.BoneName.ToString());

            return nullptr;
        }

        for (const FTransform& Key : Track.Keys)
        {
            if (Key.ContainsNaN())
                return nullptr;
        }

        TrackNames.Add(Track.BoneName);
    }

    // ---------------------------------------
    // 2. Package Path
    // ---------------------------------------

    const FString LongPackageName =
        PackagePath / AssetName;

    if (!FPackageName::IsValidLongPackageName(
        LongPackageName))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Invalid Package Name : %s"),
            *LongPackageName);

        return nullptr;
    }

    // 기존 에셋 덮어쓰기 방지

    if (FindPackage(
            nullptr,
            *LongPackageName) ||

        FPackageName::DoesPackageExist(
            LongPackageName))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation Asset Already Exists : %s"),
            *LongPackageName);

        return nullptr;
    }

    // ---------------------------------------
    // 3. Package 생성
    // ---------------------------------------

    UPackage* Package =
        CreatePackage(*LongPackageName);

    if (!Package)
        return nullptr;

    // ---------------------------------------
    // 4. AnimSequence 생성
    // ---------------------------------------

    UAnimSequence* NewAnimation =
        NewObject<UAnimSequence>(
            Package,
            FName(*AssetName),
            RF_Public |
            RF_Standalone |
            RF_Transactional
        );

    if (!NewAnimation)
        return nullptr;

    auto AbortCreation =
        [&]() -> UAnimSequence*
        {
            NewAnimation->ClearFlags(
                RF_Public | RF_Standalone
            );

            NewAnimation->MarkAsGarbage();

            Package->SetDirtyFlag(false);

            return nullptr;
        };

    NewAnimation->SetSkeleton(
        TargetSkeleton
    );

    NewAnimation->bEnableRootMotion =
        Data.bEnableRootMotion;

    // ---------------------------------------
    // 5. Animation Data Controller
    // ---------------------------------------

    IAnimationDataController& Controller =
        NewAnimation->GetController();

    Controller.InitializeModel();

    Controller.OpenBracket(
        FText::FromString(
            TEXT("Convert Animation Data")
        ),
        false
    );

    Controller.SetFrameRate(
        Data.FrameRate,
        false
    );

    // NumKeys가 아니라 NumFrames를 전달

    Controller.SetNumberOfFrames(
        FFrameNumber(Data.NumFrames),
        false
    );

    // ---------------------------------------
    // 6. Animation Track 생성
    // ---------------------------------------

    bool bBuildSuccess = true;

    for (const FAnimationConverterTrack& Track :
         Data.Tracks)
    {
        // UE5.8 : AddBoneCurve 사용

        if (!Controller.AddBoneCurve(
            Track.BoneName,
            false))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Add Bone Track Failed : %s"),
                *Track.BoneName.ToString());

            bBuildSuccess = false;
            break;
        }

        // Location / Rotation / Scale 분리

        TArray<FVector> PositionKeys;
        TArray<FQuat> RotationKeys;
        TArray<FVector> ScaleKeys;

        PositionKeys.Reserve(Data.NumKeys);
        RotationKeys.Reserve(Data.NumKeys);
        ScaleKeys.Reserve(Data.NumKeys);

        for (const FTransform& Key : Track.Keys)
        {
            FQuat Rotation =
                Key.GetRotation();

            Rotation.Normalize();

            PositionKeys.Add(
                Key.GetLocation()
            );

            RotationKeys.Add(
                Rotation
            );

            ScaleKeys.Add(
                Key.GetScale3D()
            );
        }

        // Track Key 기록

        if (!Controller.SetBoneTrackKeys(
            Track.BoneName,
            PositionKeys,
            RotationKeys,
            ScaleKeys,
            false))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Set Bone Track Keys Failed : %s"),
                *Track.BoneName.ToString());

            bBuildSuccess = false;
            break;
        }

        UE_LOG(LogAnimationConverter, Log,
            TEXT("[Created Track] %s / Keys=%d"),
            *Track.BoneName.ToString(),
            Track.Keys.Num());
    }

    // ---------------------------------------
    // 7. Data Model 생성 완료
    // ---------------------------------------

    if (bBuildSuccess)
    {
        Controller.NotifyPopulated();
    }

    Controller.CloseBracket(false);

    if (!bBuildSuccess)
        return AbortCreation();

    NewAnimation->PostEditChange();

    // ---------------------------------------
    // 8. 생성된 Animation Data 검증
    // ---------------------------------------

    if (NewAnimation->GetSkeleton() !=
        TargetSkeleton ||
        !ValidateAnimationAsset(
            NewAnimation,
            Data))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Created Animation Validation Failed"));

        return AbortCreation();
    }

    // ---------------------------------------
    // 9. Package 저장
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

    NewAnimation->MarkPackageDirty();

    FSavePackageArgs SaveArgs;

    SaveArgs.TopLevelFlags =
        RF_Public | RF_Standalone;

    SaveArgs.SaveFlags = SAVE_None;

    const bool bSaved =
        UPackage::SavePackage(
            Package,
            NewAnimation,
            *FileName,
            SaveArgs
        );

    if (!bSaved)
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Save Animation Package Failed : %s"),
            *FileName);

        return AbortCreation();
    }

    // ---------------------------------------
    // 10. Asset Registry 등록
    // ---------------------------------------

    FAssetRegistryModule::AssetCreated(
        NewAnimation
    );

    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Create AnimSequence Asset Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Asset : %s"),
        *NewAnimation->GetPathName());

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Saved : %s"),
        *FileName);

    UE_LOG(LogAnimationConverter, Log,
        TEXT("================================"));

    return NewAnimation;
}

void FAnimationConverter::TestCreate(
    const TArray<FString>& Args)
{
    if (Args.Num() < 3 ||
        Args.Num() > 4)
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Usage: AnimationConverter.Create ")
            TEXT("AnimationPath TargetSkeletonPath ")
            TEXT("OutputFolder [AssetName]"));

        return;
    }

    // ---------------------------------------
    // 1. Source Animation
    // ---------------------------------------

    UAnimSequence* SourceAnimation =
        LoadObject<UAnimSequence>(
            nullptr,
            *Args[0]
        );

    // ---------------------------------------
    // 2. Target Skeleton
    // ---------------------------------------

    USkeleton* TargetSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(SourceAnimation) ||
        !IsValid(TargetSkeleton))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Source Animation or Target Skeleton Invalid"));

        return;
    }

    // ---------------------------------------
    // 3. 저장 경로
    // ---------------------------------------

    const FString PackagePath =
        Args[2];

    const FString AssetName =
        Args.Num() == 4
            ? Args[3]
            : SourceAnimation->GetName() +
                TEXT("_Converted");

    // ---------------------------------------
    // 4. Source Animation 추출
    // ---------------------------------------

    FAnimationConverterData SourceData;

    if (!ExtractAnimationData(
        SourceAnimation,
        SourceData))
    {
        return;
    }

    // ---------------------------------------
    // 5. 151 -> 52 Bone Animation 변환
    // ---------------------------------------

    FAnimationConverterData TargetData;

    if (!ConvertAnimationData(
        SourceData,
        TargetSkeleton,
        TargetData))
    {
        return;
    }

    // ---------------------------------------
    // 6. AnimSequence 에셋 생성
    // ---------------------------------------

    UAnimSequence* NewAnimation =
        CreateAnimSequenceAsset(
            TargetData,
            TargetSkeleton,
            PackagePath,
            AssetName
        );

    if (!NewAnimation)
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Create Converted AnimSequence Failed"));

        return;
    }

    // ---------------------------------------
    // 7. 생성된 에셋 데이터 재추출
    // ---------------------------------------

    FAnimationConverterData SavedData;

    if (!ExtractAnimationData(
        NewAnimation,
        SavedData))
    {
        return;
    }

    DumpAnimationData(SavedData);

    const FString FilePath =
        FPaths::Combine(
            FPaths::ProjectSavedDir(),
            TEXT("SkeletonConverter"),
            TEXT("AnimC_Asset.json")
        );

    if (!ExportJson(
        SavedData,
        FilePath))
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Export Animation JSON Failed"));
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation Create Test Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Created Asset : %s"),
        *NewAnimation->GetPathName());
}

void FAnimationConverter::TestConvert(
    const TArray<FString>& Args)
{
    if (Args.Num() != 2)
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Usage: AnimationConverter.Convert ")
            TEXT("AnimationPath TargetSkeletonPath"));

        return;
    }

    // 1. Source Animation

    UAnimSequence* Animation =
        LoadObject<UAnimSequence>(
            nullptr,
            *Args[0]
        );

    // 2. Target Skeleton

    USkeleton* TargetSkeleton =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(Animation) ||
        !IsValid(TargetSkeleton))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation or Skeleton Load Failed"));

        return;
    }

    // 3. Source Data

    FAnimationConverterData SourceData;

    if (!ExtractAnimationData(
        Animation,
        SourceData))
    {
        return;
    }

    // 4. Convert Animation

    FAnimationConverterData TargetData;

    if (!ConvertAnimationData(
        SourceData,
        TargetSkeleton,
        TargetData))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation Conversion Failed"));

        return;
    }

    // 5. 저장된 Target Track 재평가 검증

    FAnimationConverterMapping Mapping;

    if (!BuildAnimationMapping(
        SourceData,
        TargetSkeleton,
        Mapping))
    {
        return;
    }

    TArray<FSkeletonConverterBone> SourcePose;
    TArray<FSkeletonConverterBone> TargetPose;

    for (int32 FrameIndex = 0;
         FrameIndex < TargetData.NumKeys;
         ++FrameIndex)
    {
        if (!EvaluateSourcePose(
                SourceData,
                FrameIndex,
                SourcePose) ||

            !EvaluateSourcePose(
                TargetData,
                FrameIndex,
                TargetPose) ||

            !ValidateConvertedPose(
                SourcePose,
                TargetPose,
                Mapping))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Converted Track Validation Failed : %d"),
                FrameIndex);

            return;
        }
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Validate All Converted Frames Success : %d"),
        TargetData.NumKeys);

    // 6. Target Data Dump

    DumpAnimationData(TargetData);

    // 7. JSON Export

    const FString FilePath =
        FPaths::Combine(
            FPaths::ProjectSavedDir(),
            TEXT("SkeletonConverter"),
            TEXT("AnimC.json")
        );

    if (!ExportJson(
        TargetData,
        FilePath))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Export Converted Animation Failed"));

        return;
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation Conversion Test Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Saved : %s"),
        *FilePath);
}

void FAnimationConverter::Test(
    const TArray<FString>& Args)
{
    if (Args.IsEmpty())
    {
        UE_LOG(LogAnimationConverter, Warning,
            TEXT("Usage: AnimationConverter.Test AssetPath"));

        return;
    }

    UAnimSequence* Animation =
        LoadObject<UAnimSequence>(
            nullptr,
            *Args[0]
        );

    if (!IsValid(Animation))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Animation Load Failed"));

        return;
    }

    FAnimationConverterData Data;

    if (!ExtractAnimationData(
        Animation,
        Data))
    {
        return;
    }

    DumpAnimationData(Data);

    // 전체 프레임 평가 테스트

    TArray<FSkeletonConverterBone> PoseBones;

    for (int32 FrameIndex = 0;
         FrameIndex < Data.NumKeys;
         ++FrameIndex)
    {
        if (!EvaluateSourcePose(
            Data,
            FrameIndex,
            PoseBones))
        {
            UE_LOG(LogAnimationConverter, Error,
                TEXT("Pose Evaluation Failed : Frame %d"),
                FrameIndex);

            return;
        }
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Evaluate All Frames Success : %d Keys"),
        Data.NumKeys);

    const FString FilePath =
        FPaths::Combine(
            FPaths::ProjectSavedDir(),
            TEXT("SkeletonConverter"),
            TEXT("AnimA.json")
        );

    if (!ExportJson(
        Data,
        FilePath))
    {
        UE_LOG(LogAnimationConverter, Error,
            TEXT("Export Animation JSON Failed"));

        return;
    }

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Animation Converter Test Success"));

    UE_LOG(LogAnimationConverter, Log,
        TEXT("Saved : %s"),
        *FilePath);
}