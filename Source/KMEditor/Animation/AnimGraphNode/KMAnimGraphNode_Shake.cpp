#include "KMAnimGraphNode_Shake.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UKMAnimGraphNode_Shake::UKMAnimGraphNode_Shake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UKMAnimGraphNode_Shake::GetNodeTitleColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f);
}

FText UKMAnimGraphNode_Shake::GetTooltipText() const
{
	return LOCTEXT("AnimSlotNode_Tooltip", "Screen Space Shake Node");
}

FText UKMAnimGraphNode_Shake::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return LOCTEXT("ShakeNodeTitle_NoName", "Shake");
}

FString UKMAnimGraphNode_Shake::GetNodeCategory() const
{
	return TEXT("Animation|Montage");
}

void UKMAnimGraphNode_Shake::BakeDataDuringCompilation(class FCompilerResultsLog& messageLog)
{
	UAnimBlueprint* animBlueprint = GetAnimBlueprint();
	if (!GIsCookerLoadingPackage && animBlueprint->TargetSkeleton)
	{
	}
}

#undef LOCTEXT_NAMESPACE
