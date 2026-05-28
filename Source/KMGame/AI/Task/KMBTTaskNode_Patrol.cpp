#include "KMBTTaskNode_Patrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_Patrol::UKMBTTaskNode_Patrol(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "Patrol";
}

EBTNodeResult::Type UKMBTTaskNode_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance) == true);

	if (ownerCharacterInstance->IsDead())
	{
		ownerCharacterInstance->StartForceMove(0.f);
		return EBTNodeResult::InProgress;
	}
	
	ownerCharacterInstance->StartForceMove(1.f);

	return EBTNodeResult::Succeeded;
}

void UKMBTTaskNode_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());

	if (ownerCharacterInstance->IsDead())
	{
		ownerCharacterInstance->StartForceMove(0.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
	check(IsValid(ownerCharacterInstance) == true);
}

UKMBTTaskNode_MoveStop::UKMBTTaskNode_MoveStop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveStop";
}

EBTNodeResult::Type UKMBTTaskNode_MoveStop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance) == true);

	ownerCharacterInstance->StartForceMove(0.f);

	if (ownerCharacterInstance->IsDead())
	{
		return EBTNodeResult::Aborted;
	}

	return EBTNodeResult::Succeeded;
}