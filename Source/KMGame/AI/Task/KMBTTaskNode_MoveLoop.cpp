#include "KMBTTaskNode_MoveLoop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_MoveLoop::UKMBTTaskNode_MoveLoop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveLoop";
	bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_MoveLoop::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_MoveLoop::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* sourceCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(sourceCharacterInstance));

/*	if (!sourceCharacterInstance->IsMoving())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}*/
}
