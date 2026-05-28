#include "KMBTDecorator_CanBattle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/KMBattleSubsystem.h"

UKMBTDecorator_CanBattle::UKMBTDecorator_CanBattle(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanBattle");
}

bool UKMBTDecorator_CanBattle::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UKMBattleSubsystem* battleSubsystem = UKMBattleSubsystem::GetBattleSubsystem(this);
	check(IsValid(battleSubsystem) == true);

	bool stateBattlePlaying = battleSubsystem->GetBattleState() == EKMBattleState::Playing; 
	return stateBattlePlaying;
}