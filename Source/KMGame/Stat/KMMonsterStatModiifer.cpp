#include "KMMonsterStatModifier.h"
#include "GameObject/KMCharacterInstance.h"
#include "Tables/Generated/KMTable_Character_Monster.h"
#include "Tables/Generated/KMTable_Stat_InLevelUp.h"

UKMMonsterStatModifier::UKMMonsterStatModifier(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMMonsterStatModifier::Init()
{
	Super::Init();
}

void UKMMonsterStatModifier::Deinit()
{
	Super::Deinit();
}

void UKMMonsterStatModifier::ApplyLevel(int32 newLevel, bool bCurrentFull)
{
	UKMCharacterInstance* characterObjectInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(characterObjectInstance) == true);

	const FKMTable_Character_MonsterRow* monsterRow = static_cast<const FKMTable_Character_MonsterRow*>(characterObjectInstance->GetTable());
	check(monsterRow != nullptr);
	if (monsterRow->NeedInLvUp == true)
	{
		Super::ApplyLevel(newLevel);
	}
	else
	{
		Super::ApplyLevel(0);
	}
}
void UKMMonsterStatModifier::ComputeEffectLevelStatInteral(int32 level)
{
	Super::ComputeEffectLevelStatInteral(level);

	LevelBaseStat.Multiply(FKMTable_Stat_InLevelUpRow::FindRowPtr(GetBaseStatKey(), level));
}