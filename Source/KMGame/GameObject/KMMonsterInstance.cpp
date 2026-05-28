#include "KMMonsterInstance.h"
#include "TimerManager.h"
#include "Character/KMCharacter.h"
#include "Stat/KMMonsterStatModifier.h"
#include "System/KMDropSubsystem.h"
#include "Tables/Generated/KMTable_Character_Monster.h"

UKMMonsterInstance::UKMMonsterInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	StatModifierClass = UKMMonsterStatModifier::StaticClass();
}

void UKMMonsterInstance::BeginPlay()
{
	Super::BeginPlay();
}

void UKMMonsterInstance::EndPlay()
{
	Super::EndPlay();

	if (DeathDelayTimer.IsValid() == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(DeathDelayTimer);
	}
}

void UKMMonsterInstance::OnDeath()
{
	Super::OnDeath();

	if (AKMCharacter* character = GetCharacter())
	{
		check(IsValid(character) == true);
	}	
}

bool UKMMonsterInstance::CanLockOn() const
{
	return true;
}

bool UKMMonsterInstance::CanBeTargeted() const
{
	if (IsDead() == true)
	{
		return false;
	}
	
	return true;
}

bool UKMMonsterInstance::CanReceiveReward() const
{
	const FKMTable_Character_MonsterRow* monsterTable = CastRow<FKMTable_Character_MonsterRow>(GetTable());
	check(monsterTable != nullptr);
	
	return monsterTable->NeedInLvUp;
}