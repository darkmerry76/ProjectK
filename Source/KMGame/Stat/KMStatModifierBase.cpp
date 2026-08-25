#include "KMStatModifierBase.h"
#include "DataAsset/KMAssetManager.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Skill/Ability/KMAbility.h"
#include "Tables/Generated/KMTable_BaseStat.h"

UKMStatModifierBase::UKMStatModifierBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMStatModifierBase::Init()
{
	ApplyLevel(0);

	const FKMTable_BaseStatRow* baseStatTable = FKMTable_BaseStatRow::FindRowPtr(GetBaseStatKey());
	check(baseStatTable);

	EffectiveStat.SetHpCurr(LevelBaseStat.GetHp() * baseStatTable->HpCurr);
	EffectiveStat.SetMpCurr(LevelBaseStat.GetMp() * baseStatTable->MpCurr);
}

void UKMStatModifierBase::Deinit()
{
}

void UKMStatModifierBase::Compact()
{
	const FKMTable_BaseStatRow* baseStatTable = FKMTable_BaseStatRow::FindRowPtr(GetBaseStatKey());
	check(baseStatTable);
	
	LevelBaseStat.Init(baseStatTable);
}

UKMGameObjectInstance* UKMStatModifierBase::GetOwner() const
{
	UKMGameObjectInstance* ownerGameObject = GetTypedOuter<UKMGameObjectInstance>();
	check(IsValid(ownerGameObject));

	return ownerGameObject;
}

FName UKMStatModifierBase::GetBaseStatKey() const
{
	const UKMGameObjectInstance* ownerGameObjectInstance = GetOwner();
	check(IsValid(ownerGameObjectInstance));
	return ownerGameObjectInstance->GetStatTableId();
}

void UKMStatModifierBase::ApplyLevel(int32 newLevel, bool bCurrentFull)
{
	Level = newLevel;
	
	ComputeEffectLevelStatInteral(Level);

	EffectiveStat.DuplicateValue(&LevelBaseStat);

	if (bCurrentFull)
	{
		EffectiveStat.SetHpCurr(EffectiveStat.GetHp());
		EffectiveStat.SetMpCurr(EffectiveStat.GetMp());
	}
	EffectiveStat.Sanitization();
}

void UKMStatModifierBase::ComputeEffectLevelStatInteral(int32 level)
{
	const FKMTable_BaseStatRow* baseStatTable = FKMTable_BaseStatRow::FindRowPtr(GetBaseStatKey());
	check(baseStatTable);

	LevelBaseStat.Init(baseStatTable);
}

void UKMStatModifierBase::ComputePreEffectStat()
{
	EffectiveStat.DuplicateValue(&LevelBaseStat); 
}

void UKMStatModifierBase::ComputePostEffectStat()
{
	EffectiveStat.Sanitization();
}

FKMSecondaryStat& UKMStatModifierBase::GetEffectiveStat()
{
	return EffectiveStat;
}