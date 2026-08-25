#include "KMBTTaskNode_SkillEnd.h"
#include "BehaviorTree/BlackboardComponent.h"

UKMBTTaskNode_SkillEnd::UKMBTTaskNode_SkillEnd(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "SkillEnd";
}

EBTNodeResult::Type UKMBTTaskNode_SkillEnd::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return EBTNodeResult::Succeeded;
}