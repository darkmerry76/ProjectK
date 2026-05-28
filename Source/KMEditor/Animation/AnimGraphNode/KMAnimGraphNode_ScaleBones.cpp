#include "KMAnimGraphNode_ScaleBones.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_ScaleBones"

FText UKMAnimGraphNode_ScaleBones::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ScaleBones", "Scale Bones");
}

FText UKMAnimGraphNode_ScaleBones::GetTooltipText() const
{
	return LOCTEXT("ScaleBonesTooltip", "Scale multiple bones");
}

FString UKMAnimGraphNode_ScaleBones::GetNodeCategory() const
{
	return TEXT("Custom");
}

#undef LOCTEXT_NAMESPACE