#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "KMAnimModifier_Bone.generated.h"

UENUM(BlueprintType)
enum class EKM_BoneTransformType : uint8
{
	TRANS_X,
	TRANS_Y,
	TRANS_Z,
	SCALE_X,
	SCALE_Y,
	SCALE_Z,
	ROTATION_X,
	ROTATION_Y,
	ROTATION_Z,
};

UENUM(BlueprintType)
enum class EKM_OperatorType : uint8
{
	Set,
	Add,
	Subtract,
	Multiply,
	Divide,
};

UCLASS()
class KMEDITOR_API UKMAnimModifier_Bone : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName BoneName = FName("Root");
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName TargetCurveName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Settings")
	EKM_BoneTransformType Type = EKM_BoneTransformType::TRANS_X;

	UPROPERTY(EditAnywhere, Category = "Settings")
	EKM_OperatorType OperatorType = EKM_OperatorType::Add; 
};

UCLASS()
class KMEDITOR_API UKMAnimModifier_BoneToRoot : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName BoneName = FName("Pelvis");

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 X = 0;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 Y = 1;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 Z = 2;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool CopyTranslateX = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool CopyTranslateY = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool CopyTranslateZ = true;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector DefaultTranslate;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool DefaultTranslateX = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool DefaultTranslateY = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool DefaultTranslateZ = true;
};

UCLASS()
class KMEDITOR_API UKMAnimModifier_BoneTransform : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName BoneName = FName("Pelvis");

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector AddLocation = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector MultiplyLocation = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Settings")
	FRotator AddRotator = FRotator(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector AddScale = FVector::Zero();

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector MultiplyScale = FVector(1.0f, 1.0f, 1.0f);
};

UCLASS()
class KMEDITOR_API UKMAnimModifier_FrameCut : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float StartFrame = 0.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float EndFrame = 0.f;
};

struct FKMBoneInfo
{
	FKMBoneInfo() { };
	int32 ParentBoneIndex = INDEX_NONE;
	TArray<int32> ChildBoneIndices;
	FName BoneName;
};

UCLASS()
class UKMFixPelvisYawModifier : public UAnimationModifier
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FRotator OffsetRotator = FRotator(0.f, 180, 0.f);

	TArray<FKMBoneInfo> BoneInfos;

public:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;

private:
	void FixedWorldPose(UAnimSequence* animSequence, int32 boneIndex, float time, const FTransform& parentTransform, TArray<FTransform>& outBoneWorldTransforms);
	void CreateBoneInfo(const UAnimSequence* animSequence, TArray<FKMBoneInfo>& outBoneInfos);
};

USTRUCT()
struct FKMBlendToAnimationBoneData
{
	GENERATED_USTRUCT_BODY()
	
	static FKMBlendToAnimationBoneData Create(const FName& boneName, bool bSubSearch)
	{
		FKMBlendToAnimationBoneData newData;
		newData.BoneName = boneName;
		newData.bIsSubSearch = bSubSearch;
		return newData;
	}

	UPROPERTY(EditAnywhere)
	FName BoneName;

	UPROPERTY(EditAnywhere)
	bool bIsSubSearch = true;
};

UCLASS()
class UKMBlendToAnimationModifier : public UAnimationModifier
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	UAnimSequence* TargetAnimationSequence;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FTransform TargetRootTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	float AddTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsAddTranslate = false;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsAddRotation = false;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<FKMBlendToAnimationBoneData> BlendBones;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsBlending = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float BlendingStartTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float BlendingTime = 0.2f;
	
public:
	UKMBlendToAnimationModifier();
protected:
	virtual void OnApply_Implementation(UAnimSequence* animSequence) override;
	bool IsBlendBone(const FReferenceSkeleton& refSkeleton, int32 boneIndex) const;
};