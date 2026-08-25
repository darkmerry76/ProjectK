#include "KMBTTaskNode_MoveArrival.h"
#include "BehaviorTree/BlackboardComponent.h"

UKMBTTaskNode_MoveArrival::UKMBTTaskNode_MoveArrival(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveArrival";
}

EBTNodeResult::Type UKMBTTaskNode_MoveArrival::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return EBTNodeResult::Succeeded;
}