#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode/KMAnimNode_Shake.h"
#include "KMAnimGraphNode_Shake.generated.h"

UCLASS()
class UKMAnimGraphNode_Shake : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Settings)
	FKMAnimNode_Shake Node;

	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type titleType) const override;

	virtual FString GetNodeCategory() const override;
	virtual void BakeDataDuringCompilation(class FCompilerResultsLog& messageLog) override;

private:
	FNodeTitleTextTable CachedNodeTitles;
};
