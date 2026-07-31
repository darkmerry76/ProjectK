#include "KMAnimGraphNode_MultiSlot.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UKMAnimGraphNode_MultiSlot::UKMAnimGraphNode_MultiSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UKMAnimGraphNode_MultiSlot::GetNodeTitleColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f);
}

FText UKMAnimGraphNode_MultiSlot::GetTooltipText() const
{
	return LOCTEXT("AnimSlotNode_Tooltip", "Plays animation from code using AnimMontage");
}

FText UKMAnimGraphNode_MultiSlot::GetNodeTitle(ENodeTitleType::Type titleType) const
{

	return LOCTEXT("MultiSlotNodeTitle_NoName", "MultiSlot");
}

FString UKMAnimGraphNode_MultiSlot::GetNodeCategory() const
{
	return TEXT("Animation|Montage");
}

void UKMAnimGraphNode_MultiSlot::BakeDataDuringCompilation(class FCompilerResultsLog& messageLog)
{
	UAnimBlueprint* animBlueprint = GetAnimBlueprint();
	if (!GIsCookerLoadingPackage && animBlueprint->TargetSkeleton)
	{
		for (auto nodeName : Node.SlotName)
		{
			animBlueprint->TargetSkeleton->RegisterSlotNode(nodeName);
		}
	}
}

#undef LOCTEXT_NAMESPACE
