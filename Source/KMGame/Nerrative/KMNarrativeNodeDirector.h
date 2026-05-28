#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNode.h"
#include "KMNarrativeNodeDirector.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodeDirector : public UKMNarrativeNode
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;
	
	virtual void CopyFrom(class UKMNarrativeNode* source) override;
};