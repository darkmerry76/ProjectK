#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/AnimNode/KMAnimNode_ScaleBones.h"
#include "KMAnimGraphNode_ScaleBones.generated.h"

UCLASS()
class KMEDITOR_API UKMAnimGraphNode_ScaleBones : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Settings)
	FKMAnimNode_ScaleBones Node;

protected:
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override
	{
		return &Node;
	}
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;
};