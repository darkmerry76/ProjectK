#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "EMBTServiceBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMBTServiceBase : public UBTService_BlueprintBase
{
	GENERATED_UCLASS_BODY()
	
protected:
#if WITH_EDITOR
	virtual bool UsesBlueprint() const override;
#endif

	virtual void OnSearchStart(FBehaviorTreeSearchData& searchData) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
	
	virtual void OnActivationAI(class AAIController* ownerController, class APawn* controlledPawn);
	virtual void OnActivation(class AActor* ownerActor);
	
	virtual void OnDeactivationAI(class AAIController* ownerController, class APawn* controlledPawn);
	virtual void OnDeactivation(class AActor* ownerActor);

	virtual void OnSearchStartAI(class AAIController* ownerController, class APawn* controlledPawn);
	virtual void OnSearchStart(class AActor* ownerActor);
};