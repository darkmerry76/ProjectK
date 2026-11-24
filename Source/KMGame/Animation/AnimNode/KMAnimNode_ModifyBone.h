#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "KMAnimNode_ModifyBone.generated.h"

USTRUCT()
struct KMGAME_API FKMAnimNode_ModifyBone : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletalControl, meta = (PinShownByDefault))
	FName BoneName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Translation, meta=(PinShownByDefault))
	FVector Translation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Rotation, meta=(PinShownByDefault))
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Scale, meta=(PinShownByDefault))
	FVector Scale;

	UPROPERTY(EditAnywhere, Category=Translation)
	TEnumAsByte<EBoneModificationMode> TranslationMode;

	UPROPERTY(EditAnywhere, Category=Rotation)
	TEnumAsByte<EBoneModificationMode> RotationMode;

	UPROPERTY(EditAnywhere, Category=Scale)
	TEnumAsByte<EBoneModificationMode> ScaleMode;

	UPROPERTY(EditAnywhere, Category=Translation)
	TEnumAsByte<enum EBoneControlSpace> TranslationSpace;

	UPROPERTY(EditAnywhere, Category=Rotation)
	TEnumAsByte<enum EBoneControlSpace> RotationSpace;

	UPROPERTY(EditAnywhere, Category=Scale)
	TEnumAsByte<enum EBoneControlSpace> ScaleSpace;
	
	FBoneReference DynamicBoneReference;
	
public:

	FKMAnimNode_ModifyBone();

	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;

private:
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};