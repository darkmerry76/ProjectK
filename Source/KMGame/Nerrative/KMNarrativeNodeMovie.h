#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNodeTask.h"
#include "KMNarrativeNodeMovie.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodeMovie : public UKMNarrativeNodeTask
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY()
	TObjectPtr<class UKMPrologueWindowWidget> PrologueWidget;

	float BeginTime = 0.f;
	float Duration = 0.f;

	const struct FKMTable_Narrative_MovieRow* MovieTableRow = nullptr;

	TWeakPtr<class FEMMartialArtsInstance> martialArtsInstance;

	FTimerHandle MovieTimerHandle;

public:
	virtual void CopyFrom(class UKMNarrativeNode* source) override;

protected:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsEnd() const override;
	virtual void Tick(float deltaTime) override;
};