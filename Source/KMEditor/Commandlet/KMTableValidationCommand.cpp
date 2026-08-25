#include "KMTableValidationCommand.h"
#include "DataAsset/KMAssetManager.h"
#include "Tables/Generated/KMTable_BaseStat.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Tables/Generated/KMTable_SkillEffect.h"
#include "Tables/Generated/KMTable_Stage.h"
#include "Tables/Generated/KMTable_Chapter.h"
#include "Tables/Generated/KMTable_Character_DropRate.h"
#include "Tables/Generated/KMTable_Reward.h"
#include "Tables/Generated/KMTable_StatPerLevel.h"
#include "Tables/Generated/KMTable_Stat_InLevelUp.h"
#include "Tables/Generated/KMTable_Stat_OutLevelUp.h"
#include "Tables/Generated/KMTable_Wave.h"
#include "Tables/Generated/KMTable_Object_Character.h"
#include "Tables/Generated/KMTable_Object_Character_Hero.h"
#include "Tables/Generated/KMTable_Object_Character_Monster.h"

DEFINE_LOG_CATEGORY(LogValidation)

int32 UKMTableValidationCommand::Main(const TArray<FString>& params)
{
	const UEMDataTable* characterTable = FEMDataTableHelper::Get().GetDataTables()[FKMTable_Object_CharacterRow::RefTableIndex];
	
	for (auto characterItr = characterTable->GetRowMap().CreateConstIterator(); characterItr; ++characterItr)
	{
		IsValidationCharacterTable(characterItr.Key());
	}

	return 0;
}

bool UKMTableValidationCommand::IsValidationStageTable(const FName& stageName)
{
	const FKMTable_StageRow* stageRow = FKMTable_StageRow::FindRowPtr(stageName);
	if (!stageRow)
	{
		return false;
	}

	for (FName chapterId : stageRow->ChapterList)
	{
		if (!IsValidationChapterTable(chapterId))
		{
			UE_LOG(LogValidation, Error, TEXT("stage[%s].charterId[%s] not found"), *stageName.ToString(), *chapterId.ToString());
		}
	}
	return true;
}

bool UKMTableValidationCommand::IsValidationChapterTable(const FName& chapterId)
{
	const FKMTable_ChapterRow* chapterRow = FKMTable_ChapterRow::FindRowPtr(chapterId);
	if (!chapterRow)
	{
		return false;
	}

	FString chapterMapPackageFilename, chapterMapFilename;
	bool levelExistResult = FPackageName::SearchForPackageOnDisk(chapterRow->Map.ToString(), &chapterMapPackageFilename, &chapterMapFilename);
	if (!levelExistResult)
	{
		UE_LOG(LogValidation, Error, TEXT("chapter[%s].Map[%s] Level not found"), *chapterId.ToString(), *chapterRow->Map.ToString());
	}

	for (FName waveId : chapterRow->WaveIdList)
	{
		if (!IsValidationWaveTable(waveId))
		{
			UE_LOG(LogValidation, Error, TEXT("chapter[%s].waveId[%s] not found"), *chapterId.ToString(), *waveId.ToString());
		}
	}
	return true;	
}

bool UKMTableValidationCommand::IsValidationWaveTable(const FName& waveId)
{
	const FKMTable_WaveRow* waveRow = FKMTable_WaveRow::FindRowPtr(waveId);
	if (!waveRow)
	{
		return false;
	}

	for (auto monsterId : waveRow->BossMonsterIdList)
	{
		const FKMTable_Object_CharacterRow* characterRow = FKMTable_Object_CharacterRow::FindRowPtr(monsterId);
		if(!characterRow)
		{
			UE_LOG(LogValidation, Error, TEXT("Wave[%s].BossMonsterIdList[%s] not found"), *waveId.ToString(), *monsterId.ToString());
		}
	}
	
	for (auto rewardId : waveRow->Reward)
	{
		if (!IsValidationRewardTable(rewardId))
		{
			UE_LOG(LogValidation, Error, TEXT("Wave[%s].rewardId[%s] not found"), *waveId.ToString(), *rewardId.ToString());
		}
	}
	
	if (waveRow->CharacterDropRateId != NAME_None && !IsValidationDropRateTable(waveRow->CharacterDropRateId))
	{
		UE_LOG(LogValidation, Error, TEXT("Wave[%s].CharacterDropRateId[%s] not found"), *waveId.ToString(), *waveRow->CharacterDropRateId.ToString());
	}
	
	return true;
}

bool UKMTableValidationCommand::IsValidationDropRateTable(const FName& dropRateId)
{
	const UEMDataTable* characterDropRateTable = FEMDataTableHelper::Get().GetDataTables()[FKMTable_Character_DropRateRow::RefTableIndex];
	check(characterDropRateTable);

	for (auto dropRateItr = characterDropRateTable->GetRowMap().CreateConstIterator(); dropRateItr; ++dropRateItr)
	{
		const FKMTable_Character_DropRateRow* dropRateRow = CastRow<FKMTable_Character_DropRateRow>(reinterpret_cast<FKMTable_Character_DropRateRow*>(dropRateItr.Value()));
		check(dropRateRow);

		const FKMTable_Object_CharacterRow* characterRow = FKMTable_Object_CharacterRow::FindRowPtr(dropRateRow->characterId);
		if(!characterRow)
		{
			UE_LOG(LogValidation, Error, TEXT("CharacterDropRate[%s].characterId[%s] not found"), *dropRateId.ToString(), *characterRow->Id.ToString());
		}

		if (dropRateRow->id == dropRateId)
		{
			return true;
		}
	}
	
	return false;
}

bool UKMTableValidationCommand::IsValidationRewardTable(const FName& rewardId)
{
	const FKMTable_RewardRow* rewardRow = FKMTable_RewardRow::FindRowPtr(rewardId);
	if (!rewardRow)
	{
		return false;
	}
	return true;
}

bool UKMTableValidationCommand::IsValidationCharacterTable(const FName& characterId)
{
	const FKMTable_Object_CharacterRow* characterRow = FKMTable_Object_CharacterRow::FindRowPtr(characterId);
	if (!characterRow)
	{
		UE_LOG(LogValidation, Error, TEXT("chracterRow[%s] not found"), *characterId.ToString());
		return false;
	}
	
	if (characterRow->StatId != NAME_None && !IsValidationStatTable(characterRow->StatId))
	{
		UE_LOG(LogValidation, Error, TEXT("Character[%s].StatId[%s] not found"), *characterId.ToString(), *characterRow->StatId.ToString());
	}
	
	if (UKMAssetManager::GetAssetManager()->GetPrimaryAssetPath(FPrimaryAssetId::FromString(characterRow->pdaKey.PdaKey.ToString())).IsValid() == false)
	{
		UE_LOG(LogValidation, Error, TEXT("Character[%s].pdkey[%s] PrimaryDataAsset not found"), *characterId.ToString(), *characterRow->pdaKey.PdaKey.ToString());
	}

	bool isInLevelUpCharacter = CastRow<FKMTable_Object_Character_HeroRow>(characterRow) || CastRow<FKMTable_Object_Character_MonsterRow>(characterRow);
	if (isInLevelUpCharacter)
	{
		int32 maxLevelUp = 6;
		if (const FKMTable_Object_Character_MonsterRow* monsterRow = CastRow<FKMTable_Object_Character_MonsterRow>(characterRow))
		{
			if (!monsterRow->NeedInLvUp)
			{
				maxLevelUp = 1;
			}
		}
		for (int32 inLevelUpIndex = 0;  inLevelUpIndex < maxLevelUp; ++inLevelUpIndex)
		{
			if (!FKMTable_Stat_InLevelUpRow::FindRowPtr(characterRow->StatId, inLevelUpIndex))
			{
				UE_LOG(LogValidation, Error, TEXT("Character[%s].inlevelup[%s(%d)] not found"), *characterId.ToString(), *characterRow->StatId.ToString(), inLevelUpIndex);
			}
		}
	}

	bool isOutLevelUpCharacter = CastRow<FKMTable_Object_Character_HeroRow>(characterRow) != nullptr;
	if (isOutLevelUpCharacter)
	{
		for (int32 outLevelUpIndex = 0;  outLevelUpIndex < 6; ++outLevelUpIndex)
		{
			if (!FKMTable_Stat_OutLevelUpRow::FindRowPtr(characterRow->StatId, outLevelUpIndex))
			{
				UE_LOG(LogValidation, Error, TEXT("Character[%s].outlevel[%s(%d)] not found"), *characterId.ToString(), *characterRow->StatId.ToString(), outLevelUpIndex);
			}
		}
	}

	return true;
}

bool UKMTableValidationCommand::IsValidationStatTable(const FName& statId)
{
	const FKMTable_BaseStatRow* statRow = FKMTable_BaseStatRow::FindRowPtr(statId);
	if (!statRow)
	{
		return false;
	}

	if (!IsValidationStatPerLevelTable(statRow->StatPerLevelId))
	{
		UE_LOG(LogValidation, Error, TEXT("Stat[%s].StatPerLevelId[%s] not found"), *statId.ToString(), *statRow->StatPerLevelId.ToString());
	}

	return true;
}

bool UKMTableValidationCommand::IsValidationStatPerLevelTable(const FName& statPerLevelId)
{
	const FKMTable_StatPerLevelRow* statPerLevelRow = FKMTable_StatPerLevelRow::FindRowPtr(statPerLevelId);
	if (!statPerLevelRow)
	{
		return false;
	}

	return true;
}

bool UKMTableValidationCommand::IsValidationSkillTable(const FName& skillId, int32 level)
{
	const FKMTable_SkillRow* skillRow = FKMTable_SkillRow::FindRowPtr(skillId, 0);
	if (!skillRow)
	{
		return false;
	}

	for (FName skillEffectId : skillRow->Effects)
	{
		if (!IsValidationSkillEffectTable(skillEffectId))
		{
			UE_LOG(LogValidation, Error, TEXT("skill[%s].Effect[%s] not found"), *skillId.ToString(), *skillEffectId.ToString());
		}
	}

/*	if (skillRow->Ability.IsValid() == true && IsValid(UKMAssetManager::GetAssetManager()->GetAsset(skillRow->Ability.PdaKey)) == false)
	{
		UE_LOG(LogValidation, Error, TEXT("skill[%s].Ability[%s] PrimaryDataAsset not found"), *skillId.ToString(), *skillRow->Ability.PdaKey.ToString());
	}

	if (skillRow->ProjectileBp.IsValid() == true && IsValid(UKMAssetManager::GetAssetManager()->GetAsset(skillRow->ProjectileBp.PdaKey)) == false)
	{
		UE_LOG(LogValidation, Error, TEXT("skill[%s].ProjectileBp[%s] PrimaryDataAsset not found"), *skillId.ToString(), *skillRow->ProjectileBp.PdaKey.ToString());
	}

	if (skillRow->Bp.IsValid() == true && IsValid(UKMAssetManager::GetAssetManager()->GetAsset(skillRow->Bp.PdaKey)) == false)
	{
		UE_LOG(LogValidation, Error, TEXT("skill[%s].Bp[%s] PrimaryDataAsset not found"), *skillId.ToString(), *skillRow->Bp.PdaKey.ToString());
	}*/

	return true;
}

bool UKMTableValidationCommand::IsValidationSkillEffectTable(const FName& skillEffectId)
{
	const FKMTable_SkillEffectRow* skillEffectRow = FKMTable_SkillEffectRow::FindRowPtr(skillEffectId);
	if (!skillEffectRow)
	{
		return false;
	}

	return true;
}