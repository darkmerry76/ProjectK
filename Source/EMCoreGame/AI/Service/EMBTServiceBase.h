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

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void OnActivationAI(class AAIController* OwnerController, class APawn* ControlledPawn);
	virtual void OnActivation(class AActor* OwnerActor);
	
	virtual void OnDeactivationAI(class AAIController* OwnerController, class APawn* ControlledPawn);
	virtual void OnDeactivation(class AActor* OwnerActor);

	virtual void OnSearchStartAI(class AAIController* OwnerController, class APawn* ControlledPawn);
	virtual void OnSearchStart(class AActor* OwnerActor);
};