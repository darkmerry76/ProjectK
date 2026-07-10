#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNodeTask.h"
#include "KMNarrativeNodeEvent.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodeEvent : public UKMNarrativeNodeTask
{
	GENERATED_UCLASS_BODY()

protected:
	const struct FKMTable_Narrative_EventRow* EventTableRow = nullptr;

public:
	virtual void CopyFrom(class UKMNarrativeNode* source) override;

protected:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsEnd() const override;
	virtual void Tick(float deltaTime) override;
};