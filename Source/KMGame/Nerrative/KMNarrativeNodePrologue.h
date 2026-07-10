#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNodeTask.h"
#include "KMNarrativeNodePrologue.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodePrologue : public UKMNarrativeNodeTask
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY()
	TObjectPtr<class UKMPrologueWindowWidget> PrologueWidget;

	float BeginTime = 0.f;

	const struct FKMTable_Narrative_PrologueRow* PrologueTableRow = nullptr;

	bool bIsFadeOut = false;
	
public:
	virtual void CopyFrom(class UKMNarrativeNode* source) override;

protected:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsEnd() const override;
	virtual void Tick(float deltaTime) override;
};