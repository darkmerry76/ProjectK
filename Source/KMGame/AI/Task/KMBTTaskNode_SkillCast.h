#pragma once

#include "CoreMinimal.h"
#include "KMBTTaskNode.h"
#include "KMBTTaskNode_SkillCast.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTTaskNode_SkillCast : public UKMBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTTaskNode_Run : public UKMBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};