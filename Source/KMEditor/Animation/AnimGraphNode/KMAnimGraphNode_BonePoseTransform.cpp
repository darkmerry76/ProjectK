#include "KMAnimGraphNode_BonePoseTransform.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UKMAnimGraphNode_BonePoseTransform::UKMAnimGraphNode_BonePoseTransform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UKMAnimGraphNode_BonePoseTransform::GetNodeTitleColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f);
}

FText UKMAnimGraphNode_BonePoseTransform::GetTooltipText() const
{
	return LOCTEXT("AnimSlotNode_Tooltip", "BonePostTransform Node");
}

FText UKMAnimGraphNode_BonePoseTransform::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return LOCTEXT("ShakeNodeTitle_NoName", "BonePoseTransform");
}

FString UKMAnimGraphNode_BonePoseTransform::GetNodeCategory() const
{
	return TEXT("Animation|Montage");
}

void UKMAnimGraphNode_BonePoseTransform::BakeDataDuringCompilation(class FCompilerResultsLog& messageLog)
{
	UAnimBlueprint* animBlueprint = GetAnimBlueprint();
	if (!GIsCookerLoadingPackage && animBlueprint->TargetSkeleton)
	{
	}
}

#undef LOCTEXT_NAMESPACE
