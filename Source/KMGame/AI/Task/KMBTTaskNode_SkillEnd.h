#pragma once

#include "CoreMinimal.h"
#include "KMBTTaskNode.h"
#include "KMBTTaskNode_SkillEnd.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTTaskNode_SkillEnd : public UKMBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};