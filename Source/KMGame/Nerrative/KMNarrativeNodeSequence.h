#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNode.h"
#include "KMNarrativeNodeSequence.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodeSequence : public UKMNarrativeNode
{
	GENERATED_UCLASS_BODY()

public:
	virtual void CopyFrom(class UKMNarrativeNode* source) override;

protected:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsEnd() const override;
};