#include "KMBTTaskNode_MoveArrival.h"
#include "BehaviorTree/BlackboardComponent.h"

UKMBTTaskNode_MoveArrival::UKMBTTaskNode_MoveArrival(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveArrival";
}

EBTNodeResult::Type UKMBTTaskNode_MoveArrival::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}