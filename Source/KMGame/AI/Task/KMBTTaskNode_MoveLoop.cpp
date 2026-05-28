#include "KMBTTaskNode_MoveLoop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_MoveLoop::UKMBTTaskNode_MoveLoop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveLoop";
	bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_MoveLoop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_MoveLoop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UKMCharacterInstance* sourceCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(sourceCharacterInstance) == true);

/*	if (sourceCharacterInstance->IsMoving() == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}*/
}
