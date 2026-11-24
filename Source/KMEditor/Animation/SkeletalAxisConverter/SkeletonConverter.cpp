#include "SkeletonConverter.h"
#include "Math/RotationMatrix.h"
#include "Animation/Skeleton.h"
#include "ReferenceSkeleton.h"
#include "Editor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#include "Engine/SkeletalMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/PackageName.h"

static FAutoConsoleCommand
    GSkeletonConverterTestCommand(
        TEXT("SkeletonConverter.Test"),
        TEXT("Test Skeleton Converter"),
        FConsoleCommandDelegate::CreateStatic(
            &FSkeletonConverter::Test
        )
    );

static FAutoConsoleCommand
    GSkeletonConverterDrawCommand(
        TEXT("SkeletonConverter.Draw"),
        TEXT("Draw A, B, C Skeleton"),
        FConsoleCommandDelegate::CreateStatic(
            &FSkeletonConverter::TestDebugDraw
        )
    );

static FAutoConsoleCommand
    GSkeletonConverterClearDrawCommand(
        TEXT("SkeletonConverter.ClearDraw"),
        TEXT("Clear Skeleton Debug Draw"),
        FConsoleCommandDelegate::CreateStatic(
            &FSkeletonConverter::ClearDebugDraw
        )
    );

static FAutoConsoleCommand
    GSkeletonConverterRotateCommand(
        TEXT("SkeletonConverter.Rotate"),
        TEXT("Rotate Bone : BoneName X/Y/Z Angle"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FSkeletonConverter::TestBoneRotation
        )
    );

static FAutoConsoleCommand
    GSkeletonConverterConvertCommand(
        TEXT("SkeletonConverter.Convert"),
        TEXT("Convert Source Skeleton To Reference Skeleton"),
        FConsoleCommandWithArgsDelegate::CreateStatic(
            &FSkeletonConverter::CommandConvert
        )
    );

DEFINE_LOG_CATEGORY_STATIC(
    LogSkeletonConverter, Log, All);

static const FSkeletonConverterMapping GBoneMapping[] =
{
    // Root
    { TEXT("Root"), TEXT("root") },

    // Body
    { TEXT("Bip001-Pelvis"), TEXT("pelvis")   },
    { TEXT("Bip001-Spine"),  TEXT("spine_01") },
    { TEXT("Bip001-Spine1"), TEXT("spine_05") },
    { TEXT("Bip001-Neck"),   TEXT("neck_01")  },
    { TEXT("Bip001-Head"),   TEXT("head")     },

    // Left Arm
    { TEXT("Bip001-R-Clavicle"), TEXT("clavicle_l") },
    { TEXT("Bip001-R-UpperArm"), TEXT("upperarm_l") },
    { TEXT("Bip001-R-Forearm"),  TEXT("lowerarm_l") },
    { TEXT("Bip001-R-Hand"),     TEXT("hand_l")     },

    // Left Thumb
    { TEXT("rthumb1"), TEXT("thumb_01_l") },
    { TEXT("rthumb2"), TEXT("thumb_02_l") },
    { TEXT("rthumb3"), TEXT("thumb_03_l") },

    // Left Index
    { TEXT("rindex1"), TEXT("index_01_l") },
    { TEXT("rindex2"), TEXT("index_02_l") },
    { TEXT("rindex3"), TEXT("index_03_l") },

    // Left Middle
    { TEXT("rmiddle1"), TEXT("middle_01_l") },
    { TEXT("rmiddle2"), TEXT("middle_02_l") },
    { TEXT("rmiddle3"), TEXT("middle_03_l") },

    // Left Ring
    { TEXT("rring1"), TEXT("ring_01_l") },
    { TEXT("rring2"), TEXT("ring_02_l") },
    { TEXT("rring3"), TEXT("ring_03_l") },

    // Left Pinky
    { TEXT("rpinky1"), TEXT("pinky_01_l") },
    { TEXT("rpinky2"), TEXT("pinky_02_l") },
    { TEXT("rpinky3"), TEXT("pinky_03_l") },

    // Right Arm
    { TEXT("Bip001-L-Clavicle"), TEXT("clavicle_r") },
    { TEXT("Bip001-L-UpperArm"), TEXT("upperarm_r") },
    { TEXT("Bip001-L-Forearm"),  TEXT("lowerarm_r") },
    { TEXT("Bip001-L-Hand"),     TEXT("hand_r")     },

    // Right Thumb
    { TEXT("lthumb1"), TEXT("thumb_01_r") },
    { TEXT("lthumb2"), TEXT("thumb_02_r") },
    { TEXT("lthumb3"), TEXT("thumb_03_r") },

    // Right Index
    { TEXT("lindex1"), TEXT("index_01_r") },
    { TEXT("lindex2"), TEXT("index_02_r") },
    { TEXT("lindex3"), TEXT("index_03_r") },

    // Right Middle
    { TEXT("lmiddle1"), TEXT("middle_01_r") },
    { TEXT("lmiddle2"), TEXT("middle_02_r") },
    { TEXT("lmiddle3"), TEXT("middle_03_r") },

    // Right Ring
    { TEXT("lring1"), TEXT("ring_01_r") },
    { TEXT("lring2"), TEXT("ring_02_r") },
    { TEXT("lring3"), TEXT("ring_03_r") },

    // Right Pinky
    { TEXT("lpinky1"), TEXT("pinky_01_r") },
    { TEXT("lpinky2"), TEXT("pinky_02_r") },
    { TEXT("lpinky3"), TEXT("pinky_03_r") },

    // Left Leg
    { TEXT("Bip001-L-Thigh"), TEXT("thigh_l") },
    { TEXT("Bip001-L-Calf"),  TEXT("calf_l")  },
    { TEXT("Bip001-L-Foot"),  TEXT("foot_l")  },
    { TEXT("rtoe"),          TEXT("ball_l")  },

    // Right Leg
    { TEXT("Bip001-R-Thigh"), TEXT("thigh_r") },
    { TEXT("Bip001-R-Calf"),  TEXT("calf_r")  },
    { TEXT("Bip001-R-Foot"),  TEXT("foot_r")  },
    { TEXT("ltoe"),          TEXT("ball_r")  },
};

TConstArrayView<FSkeletonConverterMapping> FSkeletonConverter::GetBoneMapping()
{
    return MakeArrayView(GBoneMapping);
}

static_assert(UE_ARRAY_COUNT(GBoneMapping) == 52);

static const FSkeletonAxisMapping GAxisMapping[] =
{
    // Body
    { TEXT("pelvis"),   TEXT("spine_01"), false },
    { TEXT("spine_01"), TEXT("spine_05"), false },
    { TEXT("spine_05"), TEXT("neck_01"),  false },
    { TEXT("neck_01"),  TEXT("head"),     false },
    { TEXT("head"),     TEXT("neck_01"),  true  },

    // Left Arm
    { TEXT("clavicle_l"), TEXT("upperarm_l"), false },
    { TEXT("upperarm_l"), TEXT("lowerarm_l"), false },
    { TEXT("lowerarm_l"), TEXT("hand_l"),     false },
    { TEXT("hand_l"),     TEXT("middle_01_l"),false },

    // Left Thumb
    { TEXT("thumb_01_l"), TEXT("thumb_02_l"), false },
    { TEXT("thumb_02_l"), TEXT("thumb_03_l"), false },
    { TEXT("thumb_03_l"), TEXT("thumb_02_l"), true  },

    // Left Index
    { TEXT("index_01_l"), TEXT("index_02_l"), false },
    { TEXT("index_02_l"), TEXT("index_03_l"), false },
    { TEXT("index_03_l"), TEXT("index_02_l"), true  },

    // Left Middle
    { TEXT("middle_01_l"), TEXT("middle_02_l"), false },
    { TEXT("middle_02_l"), TEXT("middle_03_l"), false },
    { TEXT("middle_03_l"), TEXT("middle_02_l"), true  },

    // Left Ring
    { TEXT("ring_01_l"), TEXT("ring_02_l"), false },
    { TEXT("ring_02_l"), TEXT("ring_03_l"), false },
    { TEXT("ring_03_l"), TEXT("ring_02_l"), true  },

    // Left Pinky
    { TEXT("pinky_01_l"), TEXT("pinky_02_l"), false },
    { TEXT("pinky_02_l"), TEXT("pinky_03_l"), false },
    { TEXT("pinky_03_l"), TEXT("pinky_02_l"), true  },

    // Right Arm
    { TEXT("clavicle_r"), TEXT("upperarm_r"), false },
    { TEXT("upperarm_r"), TEXT("lowerarm_r"), false },
    { TEXT("lowerarm_r"), TEXT("hand_r"),     false },
    { TEXT("hand_r"),     TEXT("middle_01_r"),false },

    // Right Thumb
    { TEXT("thumb_01_r"), TEXT("thumb_02_r"), false },
    { TEXT("thumb_02_r"), TEXT("thumb_03_r"), false },
    { TEXT("thumb_03_r"), TEXT("thumb_02_r"), true  },

    // Right Index
    { TEXT("index_01_r"), TEXT("index_02_r"), false },
    { TEXT("index_02_r"), TEXT("index_03_r"), false },
    { TEXT("index_03_r"), TEXT("index_02_r"), true  },

    // Right Middle
    { TEXT("middle_01_r"), TEXT("middle_02_r"), false },
    { TEXT("middle_02_r"), TEXT("middle_03_r"), false },
    { TEXT("middle_03_r"), TEXT("middle_02_r"), true  },

    // Right Ring
    { TEXT("ring_01_r"), TEXT("ring_02_r"), false },
    { TEXT("ring_02_r"), TEXT("ring_03_r"), false },
    { TEXT("ring_03_r"), TEXT("ring_02_r"), true  },

    // Right Pinky
    { TEXT("pinky_01_r"), TEXT("pinky_02_r"), false },
    { TEXT("pinky_02_r"), TEXT("pinky_03_r"), false },
    { TEXT("pinky_03_r"), TEXT("pinky_02_r"), true  },

    // Left Leg
    { TEXT("thigh_l"), TEXT("calf_l"), false },
    { TEXT("calf_l"),  TEXT("foot_l"), false },
    { TEXT("foot_l"),  TEXT("ball_l"), false },
    { TEXT("ball_l"),  TEXT("foot_l"), true  },

    // Right Leg
    { TEXT("thigh_r"), TEXT("calf_r"), false },
    { TEXT("calf_r"),  TEXT("foot_r"), false },
    { TEXT("foot_r"),  TEXT("ball_r"), false },
    { TEXT("ball_r"),  TEXT("foot_r"), true  },
};

static_assert(UE_ARRAY_COUNT(GAxisMapping) == 51);
    
static TSharedPtr<FJsonObject> VectorToJson(
    const FVector& Value)
{
    TSharedPtr<FJsonObject> Json =
        MakeShared<FJsonObject>();

    Json->SetNumberField(TEXT("X"), Value.X);
    Json->SetNumberField(TEXT("Y"), Value.Y);
    Json->SetNumberField(TEXT("Z"), Value.Z);

    return Json;
}

static TSharedPtr<FJsonObject> TransformToJson(
    const FTransform& Transform)
{
    TSharedPtr<FJsonObject> Json =
        MakeShared<FJsonObject>();

    const FQuat Quat = Transform.GetRotation();
    const FRotator Rotator = Quat.Rotator();

    TSharedPtr<FJsonObject> Rotation =
        MakeShared<FJsonObject>();

    Rotation->SetNumberField(TEXT("X"), Quat.X);
    Rotation->SetNumberField(TEXT("Y"), Quat.Y);
    Rotation->SetNumberField(TEXT("Z"), Quat.Z);
    Rotation->SetNumberField(TEXT("W"), Quat.W);

    TSharedPtr<FJsonObject> Euler =
        MakeShared<FJsonObject>();

    Euler->SetNumberField(TEXT("Pitch"), Rotator.Pitch);
    Euler->SetNumberField(TEXT("Yaw"), Rotator.Yaw);
    Euler->SetNumberField(TEXT("Roll"), Rotator.Roll);

    Json->SetObjectField(
        TEXT("Location"),
        VectorToJson(Transform.GetLocation()));

    Json->SetObjectField(
        TEXT("Rotation"),
        Rotation);

    Json->SetObjectField(
        TEXT("Euler"),
        Euler);

    Json->SetObjectField(
        TEXT("Scale"),
        VectorToJson(Transform.GetScale3D()));

    Json->SetObjectField(
        TEXT("AxisX"),
        VectorToJson(Quat.GetAxisX()));

    Json->SetObjectField(
        TEXT("AxisY"),
        VectorToJson(Quat.GetAxisY()));

    Json->SetObjectField(
        TEXT("AxisZ"),
        VectorToJson(Quat.GetAxisZ()));

    return Json;
}

bool FSkeletonConverter::ExportJson(
    const USkeleton* Skeleton,
    const FString& FilePath)
{
    TArray<FSkeletonConverterBone> Bones;

    if (!GetBoneData(Skeleton, Bones))
        return false;

    TSharedPtr<FJsonObject> Root =
        MakeShared<FJsonObject>();

    Root->SetStringField(
        TEXT("SkeletonName"),
        Skeleton->GetName());

    Root->SetStringField(
        TEXT("AssetPath"),
        Skeleton->GetPathName());

    Root->SetNumberField(
        TEXT("BoneCount"),
        Bones.Num());

    TArray<TSharedPtr<FJsonValue>> JsonBones;

    for (const FSkeletonConverterBone& Bone : Bones)
    {
        TSharedPtr<FJsonObject> JsonBone =
            MakeShared<FJsonObject>();

        JsonBone->SetNumberField(
            TEXT("Index"),
            Bone.BoneIndex);

        JsonBone->SetStringField(
            TEXT("Name"),
            Bone.BoneName.ToString());

        JsonBone->SetNumberField(
            TEXT("ParentIndex"),
            Bone.ParentIndex);

        JsonBone->SetStringField(
            TEXT("ParentName"),
            Bone.ParentName.ToString());

        JsonBone->SetObjectField(
            TEXT("LocalTransform"),
            TransformToJson(Bone.LocalTransform));

        JsonBone->SetObjectField(
            TEXT("ComponentTransform"),
            TransformToJson(Bone.ComponentTransform));

        JsonBones.Add(
            MakeShared<FJsonValueObject>(JsonBone));
    }

    Root->SetArrayField(TEXT("Bones"), JsonBones);

    FString OutputString;

    TSharedRef<TJsonWriter<>> Writer =
        TJsonWriterFactory<>::Create(&OutputString);

    if (!FJsonSerializer::Serialize(
        Root.ToSharedRef(), Writer))
    {
        return false;
    }

    IFileManager::Get().MakeDirectory(
        *FPaths::GetPath(FilePath), true);

    const bool bResult =
        FFileHelper::SaveStringToFile(
            OutputString,
            *FilePath,
            FFileHelper::EEncodingOptions::ForceUTF8);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Export JSON : %s / %s"),
        *FilePath,
        bResult ? TEXT("Success") : TEXT("Failed"));

    return bResult;
}

bool FSkeletonConverter::GetBoneData(
    const USkeleton* Skeleton,
    TArray<FSkeletonConverterBone>& OutBones)
{
    OutBones.Reset();

    if (!IsValid(Skeleton))
        return false;

    const FReferenceSkeleton& RefSkeleton =
        Skeleton->GetReferenceSkeleton();

    const TArray<FMeshBoneInfo>& BoneInfos =
        RefSkeleton.GetRawRefBoneInfo();

    const TArray<FTransform>& BonePoses =
        RefSkeleton.GetRawRefBonePose();

    const int32 BoneCount = BoneInfos.Num();

    if (BoneCount == 0 || BoneCount != BonePoses.Num())
        return false;

    OutBones.SetNum(BoneCount);

    for (int32 i = 0; i < BoneCount; ++i)
    {
        FSkeletonConverterBone& Bone = OutBones[i];

        Bone.BoneIndex = i;
        Bone.BoneName = BoneInfos[i].Name;
        Bone.ParentIndex = BoneInfos[i].ParentIndex;
        Bone.LocalTransform = BonePoses[i];

        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.ParentName = NAME_None;

            Bone.ComponentTransform =
                Bone.LocalTransform;
        }
        else
        {
            if (Bone.ParentIndex < 0 ||
                Bone.ParentIndex >= i)
            {
                OutBones.Reset();
                return false;
            }

            const FSkeletonConverterBone& Parent =
                OutBones[Bone.ParentIndex];

            Bone.ParentName = Parent.BoneName;

            Bone.ComponentTransform =
                Bone.LocalTransform *
                Parent.ComponentTransform;
        }
    }

    return true;
}

void FSkeletonConverter::DumpSkeleton(
    const USkeleton* Skeleton)
{
    TArray<FSkeletonConverterBone> Bones;

    if (!GetBoneData(Skeleton, Bones))
        return;

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("================================"));

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton : %s / Bones : %d"),
        *Skeleton->GetName(),
        Bones.Num());

    for (const FSkeletonConverterBone& Bone : Bones)
    {
        UE_LOG(LogSkeletonConverter, Log,
            TEXT("[%03d] %s / Parent : %s (%d)"),
            Bone.BoneIndex,
            *Bone.BoneName.ToString(),
            *Bone.ParentName.ToString(),
            Bone.ParentIndex);

        UE_LOG(LogSkeletonConverter, Log,
            TEXT("  Local : %s"),
            *Bone.LocalTransform.ToString());

        UE_LOG(LogSkeletonConverter, Log,
            TEXT("  Component : %s"),
            *Bone.ComponentTransform.ToString());
    }
}

bool FSkeletonConverter::ConvertHierarchy(
    const USkeleton* SourceSkeleton,
    const USkeleton* ReferenceSkeleton,
    TArray<FSkeletonConverterBone>& OutBones)
{
    OutBones.Reset();

    TArray<FSkeletonConverterBone> SourceBones;
    TArray<FSkeletonConverterBone> ReferenceBones;

    if (!GetBoneData(SourceSkeleton, SourceBones))
        return false;

    if (!GetBoneData(ReferenceSkeleton, ReferenceBones))
        return false;

    // Source Bone Name -> Index
    TMap<FName, int32> SourceIndexMap;

    for (const FSkeletonConverterBone& Bone : SourceBones)
    {
        SourceIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    // Reference Bone Name -> Index
    TMap<FName, int32> ReferenceIndexMap;

    for (const FSkeletonConverterBone& Bone : ReferenceBones)
    {
        ReferenceIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    // Target Bone Name -> Source Bone Index
    TMap<FName, int32> MappingTable;

    for (const FSkeletonConverterMapping& Mapping : GBoneMapping)
    {
        const int32* SourceIndex =
            SourceIndexMap.Find(Mapping.SourceBone);

        if (!SourceIndex)
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Missing Source Bone : %s"),
                *Mapping.SourceBone.ToString());

            return false;
        }

        if (!ReferenceIndexMap.Contains(Mapping.TargetBone))
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Missing Reference Bone : %s"),
                *Mapping.TargetBone.ToString());

            return false;
        }

        if (MappingTable.Contains(Mapping.TargetBone))
            return false;

        MappingTable.Add(
            Mapping.TargetBone,
            *SourceIndex);
    }

    // Reference Index -> New Bone Index
    TMap<int32, int32> ReferenceToNewIndex;

    TArray<FSkeletonConverterBone> NewBones;

    // B Skeleton 순서대로 새로운 Bone 생성
    for (const FSkeletonConverterBone& ReferenceBone :
         ReferenceBones)
    {
        const int32* SourceIndex =
            MappingTable.Find(ReferenceBone.BoneName);

        // 사용하지 않는 Bone 제외
        if (!SourceIndex)
            continue;

        const FSkeletonConverterBone& SourceBone =
            SourceBones[*SourceIndex];

        FSkeletonConverterBone NewBone;

        NewBone.BoneIndex = NewBones.Num();
        NewBone.BoneName = ReferenceBone.BoneName;

        // 원본 A의 Component Transform 유지
        NewBone.ComponentTransform =
            SourceBone.ComponentTransform;

        // 가장 가까운 유지 대상 부모 탐색
        int32 ParentIndex =
            ReferenceBone.ParentIndex;

        while (ParentIndex != INDEX_NONE)
        {
            if (ReferenceToNewIndex.Contains(ParentIndex))
                break;

            ParentIndex =
                ReferenceBones[ParentIndex].ParentIndex;
        }

        if (ParentIndex == INDEX_NONE)
        {
            NewBone.ParentIndex = INDEX_NONE;
            NewBone.ParentName = NAME_None;

            NewBone.LocalTransform =
                NewBone.ComponentTransform;
        }
        else
        {
            NewBone.ParentIndex =
                ReferenceToNewIndex.FindChecked(
                    ParentIndex);

            const FSkeletonConverterBone& ParentBone =
                NewBones[NewBone.ParentIndex];

            NewBone.ParentName =
                ParentBone.BoneName;

            NewBone.LocalTransform =
                NewBone.ComponentTransform
                .GetRelativeTransform(
                    ParentBone.ComponentTransform);
        }

        NewBone.LocalTransform.NormalizeRotation();

        ReferenceToNewIndex.Add(
            ReferenceBone.BoneIndex,
            NewBone.BoneIndex);

        NewBones.Add(NewBone);
    }

    if (NewBones.Num() != MappingTable.Num())
        return false;

    if (!ValidateHierarchy(NewBones))
        return false;

    OutBones = MoveTemp(NewBones);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Hierarchy Conversion Success : %d Bones"),
        OutBones.Num());

    return true;
}

bool FSkeletonConverter::ValidateHierarchy(
    const TArray<FSkeletonConverterBone>& Bones)
{
    if (Bones.IsEmpty())
        return false;

    TArray<FTransform> CalculatedTransforms;
    CalculatedTransforms.SetNum(Bones.Num());

    constexpr double PositionTolerance = 0.001;
    constexpr double RotationTolerance = 0.01;
    constexpr double ScaleTolerance = 0.0001;

    bool bSuccess = true;

    for (int32 i = 0; i < Bones.Num(); ++i)
    {
        const FSkeletonConverterBone& Bone = Bones[i];

        if (Bone.BoneIndex != i)
            return false;

        if (Bone.LocalTransform.ContainsNaN() ||
            Bone.ComponentTransform.ContainsNaN())
        {
            return false;
        }

        if (Bone.ParentIndex == INDEX_NONE)
        {
            // Root는 하나만 허용
            if (i != 0)
                return false;

            CalculatedTransforms[i] =
                Bone.LocalTransform;
        }
        else
        {
            if (Bone.ParentIndex < 0 ||
                Bone.ParentIndex >= i)
            {
                return false;
            }

            CalculatedTransforms[i] =
                Bone.LocalTransform *
                CalculatedTransforms[Bone.ParentIndex];
        }

        const FTransform& Calculated =
            CalculatedTransforms[i];

        const FTransform& Expected =
            Bone.ComponentTransform;

        if (Calculated.ContainsNaN())
            return false;

        const double PositionError =
            FVector::Distance(
                Calculated.GetLocation(),
                Expected.GetLocation());

        const double RotationError =
            FMath::RadiansToDegrees(
                Calculated.GetRotation().AngularDistance(
                    Expected.GetRotation()));

        const double ScaleError =
            FVector::Distance(
                Calculated.GetScale3D(),
                Expected.GetScale3D());

        if (PositionError > PositionTolerance ||
            RotationError > RotationTolerance ||
            ScaleError > ScaleTolerance)
        {
            bSuccess = false;

            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Transform Error : %s"),
                *Bone.BoneName.ToString());

            UE_LOG(LogSkeletonConverter, Error,
                TEXT("  Position : %.8f"),
                PositionError);

            UE_LOG(LogSkeletonConverter, Error,
                TEXT("  Rotation : %.8f"),
                RotationError);

            UE_LOG(LogSkeletonConverter, Error,
                TEXT("  Scale    : %.8f"),
                ScaleError);
        }
        else
        {
            UE_LOG(LogSkeletonConverter, Log,
                TEXT("[PASS] %s / Pos : %.8f / Rot : %.8f"),
                *Bone.BoneName.ToString(),
                PositionError,
                RotationError);
        }
    }

    return bSuccess;
}

bool FSkeletonConverter::ConvertBoneAxis(
    const USkeleton* ReferenceSkeleton,
    TArray<FSkeletonConverterBone>& InOutBones)
{
    if (InOutBones.IsEmpty())
        return false;

    TArray<FSkeletonConverterBone> ReferenceBones;

    if (!GetBoneData(
        ReferenceSkeleton,
        ReferenceBones))
    {
        return false;
    }

    // 실패 시 기존 데이터를 유지하기 위한 복사본
    TArray<FSkeletonConverterBone> NewBones =
        InOutBones;

    // B Skeleton Bone Index
    TMap<FName, int32> ReferenceIndexMap;

    for (const FSkeletonConverterBone& Bone :
         ReferenceBones)
    {
        ReferenceIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    // C Skeleton Bone Index
    TMap<FName, int32> TargetIndexMap;

    for (const FSkeletonConverterBone& Bone :
         NewBones)
    {
        TargetIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    // Axis Mapping Table
    TMap<FName, const FSkeletonAxisMapping*> AxisMap;

    for (const FSkeletonAxisMapping& Mapping :
         GAxisMapping)
    {
        AxisMap.Add(
            Mapping.BoneName,
            &Mapping);
    }

    // Component Space Rotation 변경
    for (FSkeletonConverterBone& Bone : NewBones)
    {
        const int32* ReferenceIndex =
            ReferenceIndexMap.Find(Bone.BoneName);

        if (!ReferenceIndex)
            return false;

        const FSkeletonConverterBone& ReferenceBone =
            ReferenceBones[*ReferenceIndex];

        // Root는 B의 Rotation 사용
        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.ComponentTransform.SetRotation(
                ReferenceBone.ComponentTransform
                    .GetRotation());

            continue;
        }

        const FSkeletonAxisMapping* const* MappingPtr =
            AxisMap.Find(Bone.BoneName);

        if (!MappingPtr)
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Missing Axis Mapping : %s"),
                *Bone.BoneName.ToString());

            return false;
        }

        const FSkeletonAxisMapping& Mapping =
            **MappingPtr;

        const int32* TargetDirectionIndex =
            TargetIndexMap.Find(
                Mapping.DirectionBone);

        const int32* RefDirectionIndex =
            ReferenceIndexMap.Find(
                Mapping.DirectionBone);

        if (!TargetDirectionIndex ||
            !RefDirectionIndex)
        {
            return false;
        }

        // A에서 유지된 관절 위치
        const FVector SourcePosition =
            Bone.ComponentTransform.GetLocation();

        const FVector SourceDirectionPosition =
            NewBones[*TargetDirectionIndex]
                .ComponentTransform.GetLocation();

        // B의 관절 위치
        const FVector ReferencePosition =
            ReferenceBone.ComponentTransform
                .GetLocation();

        const FVector ReferenceDirectionPosition =
            ReferenceBones[*RefDirectionIndex]
                .ComponentTransform.GetLocation();

        FVector SourceDirection =
            SourceDirectionPosition -
            SourcePosition;

        FVector ReferenceDirection =
            ReferenceDirectionPosition -
            ReferencePosition;

        if (Mapping.bUseParentDirection)
        {
            SourceDirection *= -1.0;
            ReferenceDirection *= -1.0;
        }

        if (!SourceDirection.Normalize() ||
            !ReferenceDirection.Normalize())
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Invalid Bone Direction : %s"),
                *Bone.BoneName.ToString());

            return false;
        }

        // B의 본 방향을 A의 본 방향으로 정렬
        const FQuat SwingRotation =
            FQuat::FindBetweenNormals(
                ReferenceDirection,
                SourceDirection);

        const FQuat ReferenceRotation =
            ReferenceBone.ComponentTransform
                .GetRotation();

        // 새로운 Component Rotation
        FQuat NewRotation =
            SwingRotation * ReferenceRotation;

        NewRotation.Normalize();

        if (NewRotation.ContainsNaN())
            return false;

        Bone.ComponentTransform.SetRotation(
            NewRotation);

        UE_LOG(LogSkeletonConverter, Log,
            TEXT("[Axis] %s : %s"),
            *Bone.BoneName.ToString(),
            *NewRotation.Rotator().ToString());
    }

    // 변경된 Component Transform을 기준으로
    // 모든 Local Transform 재계산
    for (FSkeletonConverterBone& Bone : NewBones)
    {
        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.LocalTransform =
                Bone.ComponentTransform;
        }
        else
        {
            const FSkeletonConverterBone& Parent =
                NewBones[Bone.ParentIndex];

            Bone.LocalTransform =
                Bone.ComponentTransform
                    .GetRelativeTransform(
                        Parent.ComponentTransform);
        }

        Bone.LocalTransform.NormalizeRotation();
    }

    // 기존 관절 위치가 변경되지 않았는지 검사
    constexpr double PositionTolerance = 0.001;

    for (int32 i = 0; i < NewBones.Num(); ++i)
    {
        const double PositionError =
            FVector::Distance(
                NewBones[i].ComponentTransform
                    .GetLocation(),
                InOutBones[i].ComponentTransform
                    .GetLocation());

        if (PositionError > PositionTolerance)
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Axis Position Error : %s"),
                *NewBones[i].BoneName.ToString());

            return false;
        }
    }

    // Local -> Component 재구성 검증
    if (!ValidateHierarchy(NewBones))
        return false;

    InOutBones = MoveTemp(NewBones);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Bone Axis Conversion Success : %d Bones"),
        InOutBones.Num());

    return true;
}

bool FSkeletonConverter::BuildReferenceSkeleton(
    const TArray<FSkeletonConverterBone>& Bones,
    FReferenceSkeleton& OutRefSkeleton)
{
    if (Bones.IsEmpty())
        return false;

    if (!ValidateHierarchy(Bones))
        return false;

    TSet<FName> BoneNames;

    for (const FSkeletonConverterBone& Bone : Bones)
    {
        if (Bone.BoneName.IsNone() ||
            BoneNames.Contains(Bone.BoneName))
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Invalid or Duplicate Bone : %s"),
                *Bone.BoneName.ToString());

            return false;
        }

        BoneNames.Add(Bone.BoneName);
    }

    FReferenceSkeleton NewRefSkeleton;

    {
        FReferenceSkeletonModifier Modifier(
            NewRefSkeleton,
            nullptr
        );

        for (const FSkeletonConverterBone& Bone : Bones)
        {
            const FMeshBoneInfo BoneInfo(
                Bone.BoneName,
                Bone.BoneName.ToString(),
                Bone.ParentIndex
            );

            FTransform LocalTransform =
                Bone.LocalTransform;

            LocalTransform.NormalizeRotation();

            Modifier.Add(
                BoneInfo,
                LocalTransform
            );
        }
    }

    if (NewRefSkeleton.GetRawBoneNum() != Bones.Num())
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Reference Skeleton Bone Count Mismatch"));

        return false;
    }

    OutRefSkeleton = MoveTemp(NewRefSkeleton);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Build Reference Skeleton Success : %d Bones"),
        OutRefSkeleton.GetRawBoneNum());

    return true;
}

USkeleton* FSkeletonConverter::CreateSkeletonAsset(
    const TArray<FSkeletonConverterBone>& Bones,
    const FString& PackagePath,
    const FString& AssetName)
{
    if (!IsInGameThread())
        return nullptr;

    // 1. Reference Skeleton 생성

    FReferenceSkeleton NewRefSkeleton;

    if (!BuildReferenceSkeleton(
        Bones,
        NewRefSkeleton))
    {
        return nullptr;
    }

    // 2. Package 경로 생성

    const FString LongPackageName =
        PackagePath / AssetName;

    if (!FPackageName::IsValidLongPackageName(
        LongPackageName))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Invalid Package Name : %s"),
            *LongPackageName);

        return nullptr;
    }

    // 기존 Asset 덮어쓰기 방지

    if (FindPackage(nullptr, *LongPackageName) ||
        FPackageName::DoesPackageExist(LongPackageName))
    {
        UE_LOG(LogSkeletonConverter, Warning,
            TEXT("Package Already Exists : %s"),
            *LongPackageName);

        return nullptr;
    }

    // 3. 임시 SkeletalMesh 생성
    // 실제 Mesh Geometry는 생성하지 않음

    TStrongObjectPtr<USkeletalMesh> TempMesh(
        NewObject<USkeletalMesh>(
            GetTransientPackage(),
            NAME_None,
            RF_Transient
        )
    );

    if (!TempMesh.IsValid())
        return nullptr;

    TempMesh->SetRefSkeleton(NewRefSkeleton);

    // 4. 신규 Skeleton Package 생성

    UPackage* Package =
        CreatePackage(*LongPackageName);

    if (!Package)
        return nullptr;

    // 5. 신규 USkeleton 생성

    USkeleton* NewSkeleton =
        NewObject<USkeleton>(
            Package,
            FName(*AssetName),
            RF_Public |
            RF_Standalone |
            RF_Transactional
        );

    if (!NewSkeleton)
        return nullptr;

    auto AbortCreation = [&]() -> USkeleton*
    {
        NewSkeleton->ClearFlags(
            RF_Public | RF_Standalone);

        NewSkeleton->MarkAsGarbage();

        return nullptr;
    };

    // 6. Reference Skeleton 및 BoneTree 생성

    if (!NewSkeleton->MergeAllBonesToBoneTree(
        TempMesh.Get(),
        false))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("MergeAllBonesToBoneTree Failed"));

        return AbortCreation();
    }

    // 7. 생성된 Skeleton 데이터 검증

    TArray<FSkeletonConverterBone> CreatedBones;

    if (!GetBoneData(NewSkeleton, CreatedBones))
        return AbortCreation();

    if (CreatedBones.Num() != Bones.Num())
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Created Skeleton Bone Count Mismatch"));

        return AbortCreation();
    }

    for (int32 i = 0; i < Bones.Num(); ++i)
    {
        const FSkeletonConverterBone& Expected =
            Bones[i];

        const FSkeletonConverterBone& Actual =
            CreatedBones[i];

        if (Expected.BoneName != Actual.BoneName ||
            Expected.ParentIndex != Actual.ParentIndex ||
            !Expected.LocalTransform.Equals(
                Actual.LocalTransform, 0.0001))
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Created Bone Mismatch : %s"),
                *Expected.BoneName.ToString());

            return AbortCreation();
        }
    }

    // 8. Package 저장

    const FString FileName =
        FPackageName::LongPackageNameToFilename(
            LongPackageName,
            FPackageName::GetAssetPackageExtension()
        );

    FSavePackageArgs SaveArgs;

    SaveArgs.TopLevelFlags =
        RF_Public | RF_Standalone;

    SaveArgs.SaveFlags = SAVE_None;

    Package->MarkPackageDirty();

    const bool bSaved =
        UPackage::SavePackage(
            Package,
            NewSkeleton,
            *FileName,
            SaveArgs
        );

    if (!bSaved)
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Skeleton Save Failed : %s"),
            *FileName);

        return AbortCreation();
    }

    // 9. Content Browser 등록

    FAssetRegistryModule::AssetCreated(
        NewSkeleton
    );

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Create Skeleton Asset Success : %s"),
        *NewSkeleton->GetPathName());

    return NewSkeleton;
}

static bool BuildPalmFrame(
    const TArray<FSkeletonConverterBone>& Bones,
    const TMap<FName, int32>& IndexMap,
    const FString& Side,
    FSkeletonPalmFrame& OutFrame)
{
    const int32* HandIndex =
        IndexMap.Find(
            FName(*FString::Printf(TEXT("hand_%s"), *Side)));

    const int32* MiddleIndex =
        IndexMap.Find(
            FName(*FString::Printf(TEXT("middle_01_%s"), *Side)));

    const int32* IndexIndex =
        IndexMap.Find(
            FName(*FString::Printf(TEXT("index_01_%s"), *Side)));

    const int32* PinkyIndex =
        IndexMap.Find(
            FName(*FString::Printf(TEXT("pinky_01_%s"), *Side)));

    if (!HandIndex ||
        !MiddleIndex ||
        !IndexIndex ||
        !PinkyIndex)
    {
        return false;
    }

    const FVector HandPosition =
        Bones[*HandIndex]
            .ComponentTransform.GetLocation();

    const FVector MiddlePosition =
        Bones[*MiddleIndex]
            .ComponentTransform.GetLocation();

    const FVector IndexPosition =
        Bones[*IndexIndex]
            .ComponentTransform.GetLocation();

    const FVector PinkyPosition =
        Bones[*PinkyIndex]
            .ComponentTransform.GetLocation();

    FVector Forward =
        MiddlePosition - HandPosition;

    FVector Across =
        IndexPosition - PinkyPosition;

    if (!Forward.Normalize())
        return false;

    // Gram-Schmidt
    // Forward와 Across를 직교하도록 보정

    Across -= Forward *
        FVector::DotProduct(Forward, Across);

    if (!Across.Normalize())
        return false;

    FVector Normal =
        FVector::CrossProduct(Forward, Across);

    if (!Normal.Normalize())
        return false;

    OutFrame.Forward = Forward;
    OutFrame.Across = Across;
    OutFrame.Normal = Normal;

    OutFrame.Rotation =
        FRotationMatrix::MakeFromXY(
            Forward,
            Across
        ).ToQuat();

    OutFrame.Rotation.Normalize();

    return !OutFrame.Rotation.ContainsNaN();
}

static bool BuildFingerFrame(
    const FVector& BonePosition,
    const FVector& ChildPosition,
    const FVector& PalmNormal,
    FQuat& OutRotation)
{
    FVector Forward =
        ChildPosition - BonePosition;

    if (!Forward.Normalize())
        return false;

    FVector Normal = PalmNormal;

    // 손가락 진행 방향에 수직하도록 보정

    Normal -= Forward *
        FVector::DotProduct(
            Forward,
            Normal
        );

    if (!Normal.Normalize())
        return false;

    OutRotation =
        FRotationMatrix::MakeFromXZ(
            Forward,
            Normal
        ).ToQuat();

    OutRotation.Normalize();

    return !OutRotation.ContainsNaN();
}

bool FSkeletonConverter::CorrectBoneTwist(
    const USkeleton* ReferenceSkeleton,
    TArray<FSkeletonConverterBone>& InOutBones)
{
    if (InOutBones.IsEmpty())
        return false;

    if (!ValidateHierarchy(InOutBones))
        return false;

    TArray<FSkeletonConverterBone> ReferenceBones;

    if (!GetBoneData(
        ReferenceSkeleton,
        ReferenceBones))
    {
        return false;
    }

    TArray<FSkeletonConverterBone> NewBones =
        InOutBones;

    TMap<FName, int32> TargetIndexMap;
    TMap<FName, int32> ReferenceIndexMap;

    for (const FSkeletonConverterBone& Bone : NewBones)
    {
        TargetIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    for (const FSkeletonConverterBone& Bone : ReferenceBones)
    {
        ReferenceIndexMap.Add(
            Bone.BoneName,
            Bone.BoneIndex);
    }

    const FString Sides[] =
    {
        TEXT("l"),
        TEXT("r")
    };

    const FString Fingers[] =
    {
        TEXT("index"),
        TEXT("middle"),
        TEXT("ring"),
        TEXT("pinky")
    };

    int32 CorrectedCount = 0;

    for (const FString& Side : Sides)
    {
        FSkeletonPalmFrame TargetPalm;
        FSkeletonPalmFrame ReferencePalm;

        // A의 위치가 유지된 C의 손바닥 좌표계

        if (!BuildPalmFrame(
            NewBones,
            TargetIndexMap,
            Side,
            TargetPalm))
        {
            return false;
        }

        // B의 손바닥 좌표계

        if (!BuildPalmFrame(
            ReferenceBones,
            ReferenceIndexMap,
            Side,
            ReferencePalm))
        {
            return false;
        }

        // --------------------------------------
        // 1. Hand Rotation
        // --------------------------------------

        const FName HandName(
            *FString::Printf(
                TEXT("hand_%s"), *Side));

        const int32 TargetHandIndex =
            TargetIndexMap.FindChecked(HandName);

        const int32 RefHandIndex =
            ReferenceIndexMap.FindChecked(HandName);

        FSkeletonConverterBone& TargetHand =
            NewBones[TargetHandIndex];

        const FSkeletonConverterBone& RefHand =
            ReferenceBones[RefHandIndex];

        // B의 손바닥 기준 상대 Rotation

        const FQuat HandOffset =
            ReferencePalm.Rotation.Inverse() *
            RefHand.ComponentTransform.GetRotation();

        // C의 손바닥 기준으로 적용

        FQuat NewHandRotation =
            TargetPalm.Rotation * HandOffset;

        NewHandRotation.Normalize();

        TargetHand.ComponentTransform.SetRotation(
            NewHandRotation);

        ++CorrectedCount;

        // --------------------------------------
        // 2. Finger Rotation
        // --------------------------------------

        for (const FString& Finger : Fingers)
        {
            for (int32 Joint = 1; Joint <= 3; ++Joint)
            {
                const FName BoneName(
                    *FString::Printf(
                        TEXT("%s_%02d_%s"),
                        *Finger,
                        Joint,
                        *Side));

                const int32* TargetIndex =
                    TargetIndexMap.Find(BoneName);

                const int32* RefIndex =
                    ReferenceIndexMap.Find(BoneName);

                if (!TargetIndex || !RefIndex)
                    return false;

                FSkeletonConverterBone& TargetBone =
                    NewBones[*TargetIndex];

                const FSkeletonConverterBone& RefBone =
                    ReferenceBones[*RefIndex];

                FQuat TargetFrame;
                FQuat ReferenceFrame;

                if (Joint < 3)
                {
                    // 01 -> 02
                    // 02 -> 03

                    const FName ChildName(
                        *FString::Printf(
                            TEXT("%s_%02d_%s"),
                            *Finger,
                            Joint + 1,
                            *Side));

                    const int32* TargetChildIndex =
                        TargetIndexMap.Find(ChildName);

                    const int32* RefChildIndex =
                        ReferenceIndexMap.Find(ChildName);

                    if (!TargetChildIndex ||
                        !RefChildIndex)
                    {
                        return false;
                    }

                    if (!BuildFingerFrame(
                        TargetBone.ComponentTransform
                            .GetLocation(),

                        NewBones[*TargetChildIndex]
                            .ComponentTransform
                            .GetLocation(),

                        TargetPalm.Normal,

                        TargetFrame))
                    {
                        return false;
                    }

                    if (!BuildFingerFrame(
                        RefBone.ComponentTransform
                            .GetLocation(),

                        ReferenceBones[*RefChildIndex]
                            .ComponentTransform
                            .GetLocation(),

                        ReferencePalm.Normal,

                        ReferenceFrame))
                    {
                        return false;
                    }
                }
                else
                {
                    // 마지막 관절은
                    // 02 -> 03 방향을 기준으로 계산

                    const FName ParentName(
                        *FString::Printf(
                            TEXT("%s_02_%s"),
                            *Finger,
                            *Side));

                    const int32* TargetParentIndex =
                        TargetIndexMap.Find(ParentName);

                    const int32* RefParentIndex =
                        ReferenceIndexMap.Find(ParentName);

                    if (!TargetParentIndex ||
                        !RefParentIndex)
                    {
                        return false;
                    }

                    if (!BuildFingerFrame(
                        NewBones[*TargetParentIndex]
                            .ComponentTransform
                            .GetLocation(),

                        TargetBone.ComponentTransform
                            .GetLocation(),

                        TargetPalm.Normal,

                        TargetFrame))
                    {
                        return false;
                    }

                    if (!BuildFingerFrame(
                        ReferenceBones[*RefParentIndex]
                            .ComponentTransform
                            .GetLocation(),

                        RefBone.ComponentTransform
                            .GetLocation(),

                        ReferencePalm.Normal,

                        ReferenceFrame))
                    {
                        return false;
                    }
                }

                // B의 기하학적 좌표계와
                // 실제 Bone Rotation 사이의 Offset

                const FQuat RotationOffset =
                    ReferenceFrame.Inverse() *
                    RefBone.ComponentTransform
                        .GetRotation();

                // C의 기하학적 좌표계에 적용

                FQuat NewRotation =
                    TargetFrame * RotationOffset;

                NewRotation.Normalize();

                if (NewRotation.ContainsNaN())
                    return false;

                const double CorrectionAngle =
                    FMath::RadiansToDegrees(
                        TargetBone.ComponentTransform
                            .GetRotation()
                            .AngularDistance(NewRotation));

                TargetBone.ComponentTransform
                    .SetRotation(NewRotation);

                ++CorrectedCount;

                UE_LOG(LogSkeletonConverter, Log,
                    TEXT("[Twist] %s : %.4f Degrees"),
                    *BoneName.ToString(),
                    CorrectionAngle);
            }
        }
    }

    // --------------------------------------
    // 3. Local Transform 재계산
    // --------------------------------------

    for (FSkeletonConverterBone& Bone : NewBones)
    {
        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.LocalTransform =
                Bone.ComponentTransform;
        }
        else
        {
            const FSkeletonConverterBone& Parent =
                NewBones[Bone.ParentIndex];

            Bone.LocalTransform =
                Bone.ComponentTransform
                    .GetRelativeTransform(
                        Parent.ComponentTransform);
        }

        Bone.LocalTransform.NormalizeRotation();
    }

    // --------------------------------------
    // 4. 기존 위치 및 계층 보존 검증
    // --------------------------------------

    constexpr double PositionTolerance = 0.001;
    constexpr double ScaleTolerance = 0.0001;

    for (int32 i = 0; i < NewBones.Num(); ++i)
    {
        const FSkeletonConverterBone& Before =
            InOutBones[i];

        const FSkeletonConverterBone& After =
            NewBones[i];

        if (Before.BoneName != After.BoneName ||
            Before.ParentIndex != After.ParentIndex)
        {
            return false;
        }

        const double PositionError =
            FVector::Distance(
                Before.ComponentTransform.GetLocation(),
                After.ComponentTransform.GetLocation());

        const double ScaleError =
            FVector::Distance(
                Before.ComponentTransform.GetScale3D(),
                After.ComponentTransform.GetScale3D());

        if (PositionError > PositionTolerance ||
            ScaleError > ScaleTolerance)
        {
            UE_LOG(LogSkeletonConverter, Error,
                TEXT("Twist Transform Error : %s"),
                *After.BoneName.ToString());

            return false;
        }
    }

    if (CorrectedCount != 26)
        return false;

    if (!ValidateHierarchy(NewBones))
        return false;

    InOutBones = MoveTemp(NewBones);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Bone Twist Correction Success : %d Bones"),
        CorrectedCount);

    return true;
}

void FSkeletonConverter::DrawDebugSkeleton(
    UWorld* World,
    const USkeleton* Skeleton,
    const FTransform& WorldTransform,
    bool bDrawBoneNames,
    bool bDrawBoneAxes,
    float AxisSize,
    float Duration,
    FColor BoneColor)
{
    if (!IsValid(World) || !IsValid(Skeleton))
        return;

    TArray<FSkeletonConverterBone> Bones;

    if (!GetBoneData(Skeleton, Bones))
        return;

    DrawDebugSkeletonBones(
        World,
        Skeleton,
        Bones,
        WorldTransform,
        bDrawBoneNames,
        bDrawBoneAxes,
        AxisSize,
        Duration,
        BoneColor
    );
}

void FSkeletonConverter::TestDebugDraw()
{
    if (!GEditor)
        return;

    UWorld* World =
        GEditor->GetEditorWorldContext().World();

    if (!IsValid(World))
        return;

    // Skeleton A

    USkeleton* SkeletonA =
        LoadObject<USkeleton>(
            nullptr,
            TEXT("/Game/KM/World/Character/Hero/RyuX/Preset1/EvilRyu_1_rigging_0_Skeleton.EvilRyu_1_rigging_0_Skeleton")
        );

    // Skeleton B

    USkeleton* SkeletonB =
        LoadObject<USkeleton>(
            nullptr,
            TEXT("/Game/Characters/Mannequins/Meshes/SK_Mannequin.SK_Mannequin")
        );

    // Skeleton C

    USkeleton* SkeletonC =
        LoadObject<USkeleton>(
            nullptr,
            TEXT("/Game/Test/"
                 "SK_EvilRyu_Converted52_Twist_Skeleton."
                 "SK_EvilRyu_Converted52_Twist_Skeleton")
        );

    if (!SkeletonA || !SkeletonB || !SkeletonC)
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Debug Draw Skeleton Load Failed"));

        return;
    }

    // 기존 Debug Draw 제거

    FlushPersistentDebugLines(World);

    constexpr float Spacing = 300.0f;
    constexpr float Duration = -1.0f;
    constexpr float AxisSize = 5.0f;

    // A Skeleton

    DrawDebugSkeleton(
        World,
        SkeletonA,
        FTransform(
            FQuat::Identity,
            FVector(-Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        AxisSize,
        Duration,
        FColor::White
    );

    // B Skeleton

    DrawDebugSkeleton(
        World,
        SkeletonB,
        FTransform(
            FQuat::Identity,
            FVector(0, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        AxisSize,
        Duration,
        FColor::Cyan
    );

    // C Skeleton

    DrawDebugSkeleton(
        World,
        SkeletonC,
        FTransform(
            FQuat::Identity,
            FVector(Spacing, 0, 0),
            FVector::OneVector
        ),
        false,
        true,
        AxisSize,
        Duration,
        FColor::Yellow
    );

    GEditor->RedrawLevelEditingViewports();

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Debug Draw Complete"));
}

void FSkeletonConverter::ClearDebugDraw()
{
    if (!GEditor)
        return;

    UWorld* World =
        GEditor->GetEditorWorldContext().World();

    if (!IsValid(World))
        return;

    FlushPersistentDebugLines(World);
    FlushDebugStrings(World);

    GEditor->RedrawLevelEditingViewports();

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Debug Draw Cleared"));
}

void FSkeletonConverter::DrawDebugSkeletonBones(
    UWorld* World,
    const USkeleton* Skeleton,
    const TArray<FSkeletonConverterBone>& Bones,
    const FTransform& WorldTransform,
    bool bDrawBoneNames,
    bool bDrawBoneAxes,
    float AxisSize,
    float Duration,
    FColor BoneColor)
{
    if (!IsValid(World) || Bones.IsEmpty())
        return;
    
    TArray<FTransform> WorldTransforms;
    WorldTransforms.SetNum(Bones.Num());

    // Component Space -> World Space

    for (int32 i = 0; i < Bones.Num(); ++i)
    {
        WorldTransforms[i] =
            Bones[i].ComponentTransform *
            WorldTransform;
    }

    constexpr bool bPersistent = true;
    constexpr uint8 DepthPriority = 0;

    // Bone 출력

    for (int32 i = 0; i < Bones.Num(); ++i)
    {
        const FSkeletonConverterBone& Bone =
            Bones[i];

        const FTransform& BoneWorld =
            WorldTransforms[i];

        const FVector Position =
            BoneWorld.GetLocation();

        const FQuat Rotation =
            BoneWorld.GetRotation();

        const FString BoneName =
            Bone.BoneName.ToString();

        // 손가락은 작은 크기로 출력

        const bool bFinger =
            BoneName.Contains(TEXT("thumb"))  ||
            BoneName.Contains(TEXT("index"))  ||
            BoneName.Contains(TEXT("middle")) ||
            BoneName.Contains(TEXT("ring"))   ||
            BoneName.Contains(TEXT("pinky"));

        const float CurrentAxisSize =
            bFinger ? AxisSize * 0.35f : AxisSize;

        const float JointRadius =
            bFinger ? 0.3f : 0.8f;

        const float LineThickness =
            bFinger ? 0.3f : 0.8f;

        // --------------------------------
        // 1. Parent -> Child Line
        // --------------------------------

        if (Bones.IsValidIndex(Bone.ParentIndex))
        {
            const FVector ParentPosition =
                WorldTransforms[Bone.ParentIndex]
                    .GetLocation();

            DrawDebugLine(
                World,
                ParentPosition,
                Position,
                BoneColor,
                bPersistent,
                Duration,
                DepthPriority,
                LineThickness
            );
        }

        // --------------------------------
        // 2. Joint Sphere
        // --------------------------------

        DrawDebugSphere(
            World,
            Position,
            JointRadius,
            8,
            BoneColor,
            bPersistent,
            Duration,
            DepthPriority,
            0.25f
        );

        // --------------------------------
        // 3. Bone XYZ Axis
        // --------------------------------

        if (bDrawBoneAxes)
        {
            const FVector AxisX =
                Rotation.GetAxisX() * CurrentAxisSize;

            const FVector AxisY =
                Rotation.GetAxisY() * CurrentAxisSize;

            const FVector AxisZ =
                Rotation.GetAxisZ() * CurrentAxisSize;

            DrawDebugLine(
                World,
                Position,
                Position + AxisX,
                FColor::Red,
                bPersistent,
                Duration,
                DepthPriority,
                0.5f
            );

            DrawDebugLine(
                World,
                Position,
                Position + AxisY,
                FColor::Green,
                bPersistent,
                Duration,
                DepthPriority,
                0.5f
            );

            DrawDebugLine(
                World,
                Position,
                Position + AxisZ,
                FColor::Blue,
                bPersistent,
                Duration,
                DepthPriority,
                0.5f
            );
        }

        // --------------------------------
        // 4. Bone Name
        // --------------------------------

        if (bDrawBoneNames)
        {
            DrawDebugString(
                World,
                Position + FVector(0, 0, 2),
                BoneName,
                nullptr,
                BoneColor,
                Duration,
                false,
                bFinger ? 0.6f : 0.8f
            );
        }
    }

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Debug Draw : %s / %d Bones"),
        *Skeleton->GetName(),
        Bones.Num());
}


bool FSkeletonConverter::ApplyBoneRotation(
    TArray<FSkeletonConverterBone>& InOutBones,
    const FName& BoneName,
    const FVector& LocalAxis,
    float Angle)
{
    if (!ValidateHierarchy(InOutBones))
        return false;

    if (LocalAxis.ContainsNaN() ||
        !FMath::IsFinite(Angle))
    {
        return false;
    }

    const FVector Axis = LocalAxis.GetSafeNormal();

    if (Axis.IsNearlyZero())
        return false;

    // 1. 대상 Bone 검색

    const int32 TargetIndex =
        InOutBones.IndexOfByPredicate(
            [&](const FSkeletonConverterBone& Bone)
            {
                return Bone.BoneName == BoneName;
            });

    if (TargetIndex == INDEX_NONE)
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Not Found : %s"),
            *BoneName.ToString());

        return false;
    }

    TArray<FSkeletonConverterBone> NewBones =
        InOutBones;

    FSkeletonConverterBone& TargetBone =
        NewBones[TargetIndex];

    // 2. Local Rotation 변경

    const FQuat DeltaRotation(
        Axis,
        FMath::DegreesToRadians(Angle)
    );

    const FQuat OriginalRotation =
        TargetBone.LocalTransform.GetRotation();

    FQuat NewRotation =
        OriginalRotation * DeltaRotation;

    NewRotation.Normalize();

    TargetBone.LocalTransform.SetRotation(
        NewRotation
    );

    // 3. 전체 Component Transform 재계산
    // 부모가 항상 자식보다 먼저 배치되어 있음

    for (int32 i = 0; i < NewBones.Num(); ++i)
    {
        FSkeletonConverterBone& Bone =
            NewBones[i];

        if (Bone.ParentIndex == INDEX_NONE)
        {
            Bone.ComponentTransform =
                Bone.LocalTransform;
        }
        else
        {
            const FSkeletonConverterBone& Parent =
                NewBones[Bone.ParentIndex];

            Bone.ComponentTransform =
                Bone.LocalTransform *
                Parent.ComponentTransform;
        }

        Bone.ComponentTransform.NormalizeRotation();
    }

    // 4. 결과 검증

    if (!ValidateHierarchy(NewBones))
        return false;

    const double PivotError =
        FVector::Distance(
            InOutBones[TargetIndex]
                .ComponentTransform.GetLocation(),
            NewBones[TargetIndex]
                .ComponentTransform.GetLocation()
        );

    if (PivotError > 0.001)
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Pivot Position Changed"));

        return false;
    }

    InOutBones = MoveTemp(NewBones);

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Bone Rotation Success : %s / %.2f Degrees"),
        *BoneName.ToString(),
        Angle);

    return true;
}

void FSkeletonConverter::TestBoneRotation(
    const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogSkeletonConverter, Warning,
            TEXT("Usage: SkeletonConverter.Rotate "
                 "BoneName X/Y/Z Angle"));

        return;
    }

    if (!GEditor)
        return;

    UWorld* World =
        GEditor->GetEditorWorldContext().World();

    if (!IsValid(World))
        return;

    // 1. Console Argument 처리

    const FName BoneName(*Args[0]);

    const FString AxisName =
        Args[1].ToUpper();

    FVector LocalAxis = FVector::ZeroVector;

    if (AxisName == TEXT("X"))
    {
        LocalAxis = FVector(1, 0, 0);
    }
    else if (AxisName == TEXT("Y"))
    {
        LocalAxis = FVector(0, 1, 0);
    }
    else if (AxisName == TEXT("Z"))
    {
        LocalAxis = FVector(0, 0, 1);
    }
    else
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Invalid Axis : %s"),
            *AxisName);

        return;
    }

    float Angle = 0.0f;

    if (!LexTryParseString(Angle, *Args[2]) ||
        !FMath::IsFinite(Angle))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Invalid Angle"));

        return;
    }

    // 2. Skeleton C 로딩

    USkeleton* SkeletonC =
        LoadObject<USkeleton>(
            nullptr,
            TEXT("/Game/Test/"
                 "SK_EvilRyu_Converted52_Twist_Skeleton."
                 "SK_EvilRyu_Converted52_Twist_Skeleton")
        );

    if (!SkeletonC)
        return;

    // 3. Reference Pose 추출

    TArray<FSkeletonConverterBone> ReferenceBones;

    if (!GetBoneData(
        SkeletonC,
        ReferenceBones))
    {
        return;
    }

    // 4. 테스트 Pose 생성

    TArray<FSkeletonConverterBone> TestBones =
        ReferenceBones;

    if (!ApplyBoneRotation(
        TestBones,
        BoneName,
        LocalAxis,
        Angle))
    {
        return;
    }

    // 5. 기존 Debug Draw 제거

    FlushPersistentDebugLines(World);
    FlushDebugStrings(World);

    constexpr float Spacing = 150.0f;

    // 6. Reference Pose 출력

    DrawDebugSkeletonBones(
        World,
        SkeletonC,
        ReferenceBones,
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

    // 7. 회전된 Pose 출력

    DrawDebugSkeletonBones(
        World,
        SkeletonC,
        TestBones,
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

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Rotation Test : %s / %s / %.2f"),
        *BoneName.ToString(),
        *AxisName,
        Angle);
}

void FSkeletonConverter::Test()
{
    USkeleton* SkeletonA = LoadObject<USkeleton>(
        nullptr,
        TEXT("/Game/KM/World/Character/Hero/RyuX/Preset1/EvilRyu_1_rigging_0_Skeleton.EvilRyu_1_rigging_0_Skeleton")
    );

    USkeleton* SkeletonB = LoadObject<USkeleton>(
        nullptr,
        TEXT("/Game/Characters/Mannequins/Meshes/SK_Mannequin.SK_Mannequin")
    );

    if (!SkeletonA || !SkeletonB)
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Skeleton Load Failed"));

        return;
    }

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton A : %s"),
        *SkeletonA->GetName());

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton B : %s"),
        *SkeletonB->GetName());

    // Reference Skeleton 데이터 출력
    DumpSkeleton(SkeletonA);
    DumpSkeleton(SkeletonB);

    // JSON Export
    const FString SavePath = FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("SkeletonConverter")
    );

    ExportJson(
        SkeletonA,
        FPaths::Combine(SavePath, TEXT("A.json"))
    );

    ExportJson(
        SkeletonB,
        FPaths::Combine(SavePath, TEXT("B.json"))
    );
    
    TArray<FSkeletonConverterBone> ConvertedBones;

    if (!ConvertHierarchy(
        SkeletonA,
        SkeletonB,
        ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Hierarchy Conversion Failed"));

        return;
    }
    
    if (!ConvertBoneAxis(
    SkeletonB,
    ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Axis Conversion Failed"));

        return;
    }
    
    if (!CorrectBoneTwist(
    SkeletonB,
    ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Twist Correction Failed"));

        return;
    }
    
    USkeleton* SkeletonC =
        CreateSkeletonAsset(
            ConvertedBones,
            TEXT("/Game/Test"),
            TEXT("SK_EvilRyu_Converted52_Twist_Skeleton")
        );

    if (!SkeletonC)
        return;

    ExportJson(
        SkeletonC,
        FPaths::Combine(
            FPaths::ProjectSavedDir(),
            TEXT("SkeletonConverter"),
            TEXT("C52_Twist.json")
        )
    );

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Conversion All Success"));
}

USkeleton* FSkeletonConverter::Convert(
    const USkeleton* SkeletonA,
    const USkeleton* SkeletonB,
    const FString& OutputFolder,
    const FString& AssetName)
{
    if (!IsInGameThread() ||
        !IsValid(SkeletonA) ||
        !IsValid(SkeletonB) ||
        SkeletonA == SkeletonB ||
        OutputFolder.IsEmpty() ||
        AssetName.IsEmpty())
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Invalid Skeleton Conversion Parameters"));

        return nullptr;
    }

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("================================"));

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Conversion Start"));

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton A : %s"),
        *SkeletonA->GetPathName());

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton B : %s"),
        *SkeletonB->GetPathName());

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Output : %s/%s"),
        *OutputFolder,
        *AssetName);

    // 1. Hierarchy Conversion

    TArray<FSkeletonConverterBone> ConvertedBones;

    if (!ConvertHierarchy(
        SkeletonA,
        SkeletonB,
        ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Hierarchy Conversion Failed"));

        return nullptr;
    }

    // 2. Bone Axis Conversion

    if (!ConvertBoneAxis(
        SkeletonB,
        ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Axis Conversion Failed"));

        return nullptr;
    }

    // 3. Bone Twist Correction

    if (!CorrectBoneTwist(
        SkeletonB,
        ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Bone Twist Correction Failed"));

        return nullptr;
    }

    // 4. Validate Hierarchy

    if (!ValidateHierarchy(ConvertedBones))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Hierarchy Validation Failed"));

        return nullptr;
    }

    // 5. Create Skeleton Asset

    USkeleton* SkeletonC =
        CreateSkeletonAsset(
            ConvertedBones,
            OutputFolder,
            AssetName
        );

    if (!IsValid(SkeletonC))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Create Skeleton Asset Failed"));

        return nullptr;
    }

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("================================"));

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Conversion All Success"));

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton C : %s"),
        *SkeletonC->GetPathName());

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Bone Count : %d"),
        ConvertedBones.Num());

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("================================"));

    return SkeletonC;
}

void FSkeletonConverter::CommandConvert(
    const TArray<FString>& Args)
{
    if (Args.Num() != 4)
    {
        UE_LOG(LogSkeletonConverter, Warning,
            TEXT("Usage: SkeletonConverter.Convert ")
            TEXT("SkeletonA SkeletonB OutputFolder AssetName"));

        return;
    }

    // 1. Source Skeleton

    USkeleton* SkeletonA =
        LoadObject<USkeleton>(
            nullptr,
            *Args[0]
        );

    // 2. Reference Skeleton

    USkeleton* SkeletonB =
        LoadObject<USkeleton>(
            nullptr,
            *Args[1]
        );

    if (!IsValid(SkeletonA) ||
        !IsValid(SkeletonB))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Skeleton Load Failed"));

        return;
    }

    // 3. Output Parameters

    const FString& OutputFolder = Args[2];
    const FString& AssetName = Args[3];

    // 4. Convert

    USkeleton* SkeletonC =
        Convert(
            SkeletonA,
            SkeletonB,
            OutputFolder,
            AssetName
        );

    if (!IsValid(SkeletonC))
    {
        UE_LOG(LogSkeletonConverter, Error,
            TEXT("Skeleton Conversion Command Failed"));

        return;
    }

    UE_LOG(LogSkeletonConverter, Log,
        TEXT("Skeleton Conversion Command Success"));
}