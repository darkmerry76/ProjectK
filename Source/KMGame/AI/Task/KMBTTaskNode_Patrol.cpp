#include "KMBTTaskNode_Patrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_Patrol::UKMBTTaskNode_Patrol(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "Patrol";
}

EBTNodeResult::Type UKMBTTaskNode_Patrol::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	if (ownerCharacterInstance->IsDead())
	{
		ownerCharacterInstance->StartForceMove(0.f);
		return EBTNodeResult::InProgress;
	}
	
	ownerCharacterInstance->StartForceMove(1.f);

	return EBTNodeResult::Succeeded;
}

void UKMBTTaskNode_Patrol::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());

	if (ownerCharacterInstance->IsDead())
	{
		ownerCharacterInstance->StartForceMove(0.f);
		FinishLatentTask(ownerComp, EBTNodeResult::Aborted);
	}
	check(IsValid(ownerCharacterInstance));
}

UKMBTTaskNode_MoveStop::UKMBTTaskNode_MoveStop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveStop";
}

EBTNodeResult::Type UKMBTTaskNode_MoveStop::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	ownerCharacterInstance->StartForceMove(0.f);

	if (ownerCharacterInstance->IsDead())
	{
		return EBTNodeResult::Aborted;
	}

	return EBTNodeResult::Succeeded;
}