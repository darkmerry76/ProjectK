#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimTypes.h"
#include "Animation/AnimNodeBase.h"
#include "KMAnimNode_MultiSlot.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct KMGAME_API FKMAnimNode_MultiSlot : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Links)
	FPoseLink Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TArray<FName> SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bAlwaysUpdateSourcePose;

protected:
	virtual void PostEvaluateSourcePose(FPoseContext& sourceContext) {}

	bool GetBestSlotName(const FAnimInstanceProxy* animInstanceProxy, FName& outSlotName, FSlotNodeWeightInfo& outWeightData) const;

	FSlotNodeWeightInfo WeightData;
	FGraphTraversalCounter SlotNodeInitializationCounter;

public:	
	FKMAnimNode_MultiSlot();

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& context) override;
	virtual void Evaluate_AnyThread(FPoseContext& output) override;
	virtual void GatherDebugData(FNodeDebugData& debugData) override;
};
