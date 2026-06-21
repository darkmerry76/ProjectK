#include "KMStatModifierBase.h"
#include "Character/KMCharacter.h"
#include "Core/KMGameInstance.h"
#include "DataAsset/KMAssetManager.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Skill/KMSkillTypes.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilityBlow.h"
#include "Skill/Ability/KMAbilitySet.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_BaseStat.h"

UKMStatModifierBase::UKMStatModifierBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMStatModifierBase::Init()
{
	ApplyLevel(0);

	const FKMTable_BaseStatRow* baseStatTable = FKMTable_BaseStatRow::FindRowPtr(GetBaseStatKey());
	check(baseStatTable != nullptr);

	EffectiveStat.SetHpCurr(LevelBaseStat.GetHp() * baseStatTable->HpCurr);
	EffectiveStat.SetMpCurr(LevelBaseStat.GetMp() * baseStatTable->MpCurr);
}

void UKMStatModifierBase::Deinit()
{
}

UKMGameObjectInstance* UKMStatModifierBase::GetOwner() const
{
	UKMGameObjectInstance* ownerGameObject = GetTypedOuter<UKMGameObjectInstance>();
	check(IsValid(ownerGameObject) == true);

	return ownerGameObject;
}

FName UKMStatModifierBase::GetBaseStatKey() const
{
	const UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance) == true);
	return ownerCharacterInstance->GetRecordStatKey();
}

void UKMStatModifierBase::ApplyLevel(int32 newLevel, bool bCurrentFull)
{
	Level = newLevel;
	
	ComputeEffectLevelStatInteral(Level);

	EffectiveStat.DuplicateValue(&LevelBaseStat);

	if (bCurrentFull == true)
	{
		EffectiveStat.SetHpCurr(EffectiveStat.GetHp());
		EffectiveStat.SetMpCurr(EffectiveStat.GetMp());
	}
	EffectiveStat.Sanitization();
}

void UKMStatModifierBase::ComputeEffectLevelStatInteral(int32 level)
{
	const FKMTable_BaseStatRow* baseStatTable = FKMTable_BaseStatRow::FindRowPtr(GetBaseStatKey());
	check(baseStatTable != nullptr);

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

UObject* UKMStatModifierBase::ApplyEffectiveAnimation(EKMAnimSetEffectType effectType)
{
	UKMCharacterInstance* characterObjectInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(characterObjectInstance) == true);

	AKMCharacter* character = characterObjectInstance->GetCharacter();
	check(IsValid(character) == true);

	UKMGameInstance* gameInstance = UKMGameInstance::GetGameInstance(this);
	const UKMAbilityEffectSet* abilityEffectSet = gameInstance->GetAnormalAbilitySet();
	check(IsValid(abilityEffectSet));

	const TSubclassOf<UKMAbility>* abilityClass = abilityEffectSet->AbilityMap.Find(effectType);
	if (abilityClass && IsValid(*abilityClass))
	{
		UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
		check(IsValid(assetManager) == true);
			
		UKMAbility* newAbility = NewObject<UKMAbility>(this, *abilityClass);
		AbnormalAbilities.Emplace(newAbility);

		return newAbility;
	}

	return nullptr;
}

UObject* UKMStatModifierBase::ApplyEffectiveAnimation(const FName& pDAKey)
{
	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager) == true);

	UObject* assetObject = assetManager->GetAsset(pDAKey);

	UKMAbility* newAbility = nullptr;
	if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(assetObject))
	{
		if (IsValid(martialArts->GetAbilityBP()->GeneratedClass))
		{
			newAbility = NewObject<UKMAbility>(this, martialArts->GetAbilityBP()->GeneratedClass);
			newAbility->SetMartialArts(martialArts);
		}
	}
	else
	{
		UClass* abilityClass = Cast<UClass>(assetObject);
		if (!abilityClass)
		{
			return nullptr;
		}
		newAbility = NewObject<UKMAbility>(this, abilityClass);
	}
	if (!IsValid(newAbility))
	{
		return nullptr;
	}
	AbnormalAbilities.Emplace(newAbility);
	return newAbility;
}

void UKMStatModifierBase::RemoveEffectiveAnimation(class UObject* effectObject)
{
	if (AbnormalAbilities.Contains(effectObject))
	{
		if (UKMAbility* ability = Cast<UKMAbility>(effectObject))
		{
			ability->Deactivate();
		}
		AbnormalAbilities.Emplace(effectObject);
	}
}