#include "KMHeroStatModifier.h"
#include "GameObject/KMCharacterInstance.h"
#include "Tables/Generated/KMTable_PlayerLevel.h"
#include "Tables/Generated/KMTable_Stat_InLevelUp.h"
#include "Tables/Generated/KMTable_Stat_OutLevelUp.h"
#include "Util/KMUtil.h"

UKMHeroStatModifier::UKMHeroStatModifier(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHeroStatModifier::Init()
{
	Super::Init();
}

void UKMHeroStatModifier::Deinit()
{
	Super::Deinit();
}

void UKMHeroStatModifier::ComputeEffectLevelStatInteral(int32 level)
{
	Super::ComputeEffectLevelStatInteral(level);

	LevelBaseStat.Multiply(FKMTable_Stat_InLevelUpRow::FindRowPtr(GetBaseStatKey(), level));
	LevelBaseStat.Multiply(FKMTable_Stat_OutLevelUpRow::FindRowPtr(GetBaseStatKey(), level));
}