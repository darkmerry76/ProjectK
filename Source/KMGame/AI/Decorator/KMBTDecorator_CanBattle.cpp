#include "KMBTDecorator_CanBattle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/KMBattleSubsystem.h"

UKMBTDecorator_CanBattle::UKMBTDecorator_CanBattle(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanBattle");
}

bool UKMBTDecorator_CanBattle::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
{
	UKMBattleSubsystem* battleSubsystem = UKMBattleSubsystem::GetBattleSubsystem(this);
	check(IsValid(battleSubsystem));

	bool stateBattlePlaying = battleSubsystem->GetBattleState() == EKMBattleState::Playing; 
	return stateBattlePlaying;
}