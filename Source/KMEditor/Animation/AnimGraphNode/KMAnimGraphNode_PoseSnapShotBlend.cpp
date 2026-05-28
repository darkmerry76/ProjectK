#include "KMAnimGraphNode_PoseSnapShotBlend.h"

FText UKMAnimGraphNode_PoseSnapShotBlend::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("PoseSnapShotBlend");
}

FString UKMAnimGraphNode_PoseSnapShotBlend::GetNodeCategory() const
{
	return TEXT("Blend");
}