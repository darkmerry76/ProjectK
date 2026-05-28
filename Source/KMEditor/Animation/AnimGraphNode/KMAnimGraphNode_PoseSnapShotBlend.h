#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode/KMAnimNode_PoseSnapShotBlend.h"
#include "KMAnimGraphNode_PoseSnapShotBlend.generated.h"

UCLASS()
class KMEDITOR_API UKMAnimGraphNode_PoseSnapShotBlend : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Settings)
	FKMAnimNode_PoseSnapShotBlend Node;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const;
	virtual FString GetNodeCategory() const;
};