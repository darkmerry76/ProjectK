#include "KMBTTaskNode_SkillEnd.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_SkillEnd::UKMBTTaskNode_SkillEnd(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "SkillEnd";
}

EBTNodeResult::Type UKMBTTaskNode_SkillEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}