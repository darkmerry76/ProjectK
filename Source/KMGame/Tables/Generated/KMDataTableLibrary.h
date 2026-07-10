#pragma once

#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Actor.h"
#include "KMTable_Actor_VFX.h"
#include "KMTable_BaseStat.h"
#include "KMTable_BaseStat_Ghost.h"
#include "KMTable_BaseStat_Hero.h"
#include "KMTable_BaseStat_Monster.h"
#include "KMTable_Chapter.h"
#include "KMTable_Chapter_Dungeon.h"
#include "KMTable_Chapter_Raid.h"
#include "KMTable_Character.h"
#include "KMTable_Character_DropRate.h"
#include "KMTable_Character_Hero.h"
#include "KMTable_Character_Monster.h"
#include "KMTable_Drop.h"
#include "KMTable_Drop_InGameEvent.h"
#include "KMTable_Drop_InGameSkill.h"
#include "KMTable_Drop_Item.h"
#include "KMTable_GameConstant.h"
#include "KMTable_GameEvent.h"
#include "KMTable_InGameEvent.h"
#include "KMTable_Item.h"
#include "KMTable_LocalizeText.h"
#include "KMTable_LocalizeText_Desc.h"
#include "KMTable_LocalizeText_Format.h"
#include "KMTable_LocalizeText_Message.h"
#include "KMTable_LocalizeText_UI.h"
#include "KMTable_Narrative.h"
#include "KMTable_Narrative_Dialog.h"
#include "KMTable_Narrative_Director.h"
#include "KMTable_Narrative_Movie.h"
#include "KMTable_Narrative_Prologue.h"
#include "KMTable_Narrative_Sequence.h"
#include "KMTable_OutLevelupCost.h"
#include "KMTable_PlayerGrowth.h"
#include "KMTable_PlayerLevel.h"
#include "KMTable_RecommendContent.h"
#include "KMTable_Reward.h"
#include "KMTable_Reward_Character.h"
#include "KMTable_Reward_Stage.h"
#include "KMTable_Skill.h"
#include "KMTable_Skill_Normal.h"
#include "KMTable_Skill_Projectile.h"
#include "KMTable_SkillCondition.h"
#include "KMTable_SkillEffect.h"
#include "KMTable_SkillEffect_Grab.h"
#include "KMTable_SkillEffect_Normal.h"
#include "KMTable_SkillEffectTransition.h"
#include "KMTable_SkillSet.h"
#include "KMTable_SkillSet_Hero.h"
#include "KMTable_Stage.h"
#include "KMTable_Stage_Dungeon.h"
#include "KMTable_Stage_Field.h"
#include "KMTable_Stage_Raid.h"
#include "KMTable_StartBattleCharacter.h"
#include "KMTable_Stat_InLevelUp.h"
#include "KMTable_Stat_OutLevelUp.h"
#include "KMTable_StatPerLevel.h"
#include "KMTable_UnlockContent.h"
#include "KMTable_UnlockContentMainLobby.h"
#include "KMTable_Wave.h"
#include "KMTable_Wave_Dungeon.h"
#include "KMTable_Wave_Field.h"
#include "KMDataTableLibrary.generated.h"

UCLASS()
class UKMDataTableLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	template<typename _TL>
	static const _TL* GetTableEM(FName IndexName, bool& IsFind)
	{
		const _TL* TableRow = FEMDataTableHelper::Get().FindRowPtr<_TL>(IndexName);
		IsFind = (nullptr != TableRow) ? true : false;
		if(false == IsFind)
		{
			static _TL Default;
			return &Default;
		}
		return TableRow;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 블루프린트에 노출되는 함수 모음
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// KMTable_Actor FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ActorRow& GetTableKMTable_Actor(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ActorRow>(IndexName, IsFind);
	}

	// KMTable_Actor Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ActorRow& GetTableKMTable_ActorByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ActorRow>(*FKMTable_ActorRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_BaseStat FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_BaseStatRow& GetTableKMTable_BaseStat(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_BaseStatRow>(IndexName, IsFind);
	}

	// KMTable_BaseStat Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_BaseStatRow& GetTableKMTable_BaseStatByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_BaseStatRow>(*FKMTable_BaseStatRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Chapter FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ChapterRow& GetTableKMTable_Chapter(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ChapterRow>(IndexName, IsFind);
	}

	// KMTable_Chapter Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ChapterRow& GetTableKMTable_ChapterByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ChapterRow>(*FKMTable_ChapterRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Character FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_CharacterRow& GetTableKMTable_Character(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_CharacterRow>(IndexName, IsFind);
	}

	// KMTable_Character Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_CharacterRow& GetTableKMTable_CharacterByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_CharacterRow>(*FKMTable_CharacterRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Character_DropRate FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_DropRateRow& GetTableKMTable_Character_DropRate(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_DropRateRow>(IndexName, IsFind);
	}

	// KMTable_Character_DropRate Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_DropRateRow& GetTableKMTable_Character_DropRateByKey(FName idKey, FName characterIdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_DropRateRow>(*FKMTable_Character_DropRateRow::MakeTableKeyToString(idKey, characterIdKey), IsFind);
	}

	// KMTable_Character_Hero FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_HeroRow& GetTableKMTable_Character_Hero(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_HeroRow>(IndexName, IsFind);
	}

	// KMTable_Character_Hero Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_HeroRow& GetTableKMTable_Character_HeroByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_HeroRow>(*FKMTable_Character_HeroRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Character_Monster FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_MonsterRow& GetTableKMTable_Character_Monster(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_MonsterRow>(IndexName, IsFind);
	}

	// KMTable_Character_Monster Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Character_MonsterRow& GetTableKMTable_Character_MonsterByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Character_MonsterRow>(*FKMTable_Character_MonsterRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Drop FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_DropRow& GetTableKMTable_Drop(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_DropRow>(IndexName, IsFind);
	}

	// KMTable_Drop Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_DropRow& GetTableKMTable_DropByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_DropRow>(*FKMTable_DropRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Drop_InGameEvent FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_InGameEventRow& GetTableKMTable_Drop_InGameEvent(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_InGameEventRow>(IndexName, IsFind);
	}

	// KMTable_Drop_InGameEvent Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_InGameEventRow& GetTableKMTable_Drop_InGameEventByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_InGameEventRow>(*FKMTable_Drop_InGameEventRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Drop_InGameSkill FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_InGameSkillRow& GetTableKMTable_Drop_InGameSkill(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_InGameSkillRow>(IndexName, IsFind);
	}

	// KMTable_Drop_InGameSkill Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_InGameSkillRow& GetTableKMTable_Drop_InGameSkillByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_InGameSkillRow>(*FKMTable_Drop_InGameSkillRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Drop_Item FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_ItemRow& GetTableKMTable_Drop_Item(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_ItemRow>(IndexName, IsFind);
	}

	// KMTable_Drop_Item Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Drop_ItemRow& GetTableKMTable_Drop_ItemByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Drop_ItemRow>(*FKMTable_Drop_ItemRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_GameConstant FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_GameConstantRow& GetTableKMTable_GameConstant(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_GameConstantRow>(IndexName, IsFind);
	}

	// KMTable_GameConstant Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_GameConstantRow& GetTableKMTable_GameConstantByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_GameConstantRow>(*FKMTable_GameConstantRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_GameEvent FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_GameEventRow& GetTableKMTable_GameEvent(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_GameEventRow>(IndexName, IsFind);
	}

	// KMTable_GameEvent Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_GameEventRow& GetTableKMTable_GameEventByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_GameEventRow>(*FKMTable_GameEventRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_InGameEvent FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_InGameEventRow& GetTableKMTable_InGameEvent(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_InGameEventRow>(IndexName, IsFind);
	}

	// KMTable_InGameEvent Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_InGameEventRow& GetTableKMTable_InGameEventByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_InGameEventRow>(*FKMTable_InGameEventRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Item FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ItemRow& GetTableKMTable_Item(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ItemRow>(IndexName, IsFind);
	}

	// KMTable_Item Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_ItemRow& GetTableKMTable_ItemByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_ItemRow>(*FKMTable_ItemRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_LocalizeText FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_LocalizeTextRow& GetTableKMTable_LocalizeText(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_LocalizeTextRow>(IndexName, IsFind);
	}

	// KMTable_LocalizeText Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_LocalizeTextRow& GetTableKMTable_LocalizeTextByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_LocalizeTextRow>(*FKMTable_LocalizeTextRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Narrative FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_NarrativeRow& GetTableKMTable_Narrative(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_NarrativeRow>(IndexName, IsFind);
	}

	// KMTable_Narrative Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_NarrativeRow& GetTableKMTable_NarrativeByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_NarrativeRow>(*FKMTable_NarrativeRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Narrative_Dialog FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_DialogRow& GetTableKMTable_Narrative_Dialog(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_DialogRow>(IndexName, IsFind);
	}

	// KMTable_Narrative_Dialog Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_DialogRow& GetTableKMTable_Narrative_DialogByKey(FName IdKey, int32 subIdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_DialogRow>(*FKMTable_Narrative_DialogRow::MakeTableKeyToString(IdKey, subIdKey), IsFind);
	}

	// KMTable_Narrative_Director FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_DirectorRow& GetTableKMTable_Narrative_Director(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_DirectorRow>(IndexName, IsFind);
	}

	// KMTable_Narrative_Director Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_DirectorRow& GetTableKMTable_Narrative_DirectorByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_DirectorRow>(*FKMTable_Narrative_DirectorRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Narrative_Movie FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_MovieRow& GetTableKMTable_Narrative_Movie(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_MovieRow>(IndexName, IsFind);
	}

	// KMTable_Narrative_Movie Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_MovieRow& GetTableKMTable_Narrative_MovieByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_MovieRow>(*FKMTable_Narrative_MovieRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Narrative_Prologue FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_PrologueRow& GetTableKMTable_Narrative_Prologue(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_PrologueRow>(IndexName, IsFind);
	}

	// KMTable_Narrative_Prologue Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_PrologueRow& GetTableKMTable_Narrative_PrologueByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_PrologueRow>(*FKMTable_Narrative_PrologueRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Narrative_Sequence FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_SequenceRow& GetTableKMTable_Narrative_Sequence(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_SequenceRow>(IndexName, IsFind);
	}

	// KMTable_Narrative_Sequence Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Narrative_SequenceRow& GetTableKMTable_Narrative_SequenceByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Narrative_SequenceRow>(*FKMTable_Narrative_SequenceRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_OutLevelupCost FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_OutLevelupCostRow& GetTableKMTable_OutLevelupCost(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_OutLevelupCostRow>(IndexName, IsFind);
	}

	// KMTable_OutLevelupCost Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_OutLevelupCostRow& GetTableKMTable_OutLevelupCostByKey(int32 IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_OutLevelupCostRow>(*FKMTable_OutLevelupCostRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_PlayerGrowth FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_PlayerGrowthRow& GetTableKMTable_PlayerGrowth(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_PlayerGrowthRow>(IndexName, IsFind);
	}

	// KMTable_PlayerGrowth Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_PlayerGrowthRow& GetTableKMTable_PlayerGrowthByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_PlayerGrowthRow>(*FKMTable_PlayerGrowthRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_PlayerLevel FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_PlayerLevelRow& GetTableKMTable_PlayerLevel(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_PlayerLevelRow>(IndexName, IsFind);
	}

	// KMTable_PlayerLevel Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_PlayerLevelRow& GetTableKMTable_PlayerLevelByKey(int32 IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_PlayerLevelRow>(*FKMTable_PlayerLevelRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_RecommendContent FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_RecommendContentRow& GetTableKMTable_RecommendContent(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_RecommendContentRow>(IndexName, IsFind);
	}

	// KMTable_RecommendContent Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_RecommendContentRow& GetTableKMTable_RecommendContentByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_RecommendContentRow>(*FKMTable_RecommendContentRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Reward FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_RewardRow& GetTableKMTable_Reward(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_RewardRow>(IndexName, IsFind);
	}

	// KMTable_Reward Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_RewardRow& GetTableKMTable_RewardByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_RewardRow>(*FKMTable_RewardRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Skill FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillRow& GetTableKMTable_Skill(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillRow>(IndexName, IsFind);
	}

	// KMTable_Skill Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillRow& GetTableKMTable_SkillByKey(FName IdKey, int32 LevelKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillRow>(*FKMTable_SkillRow::MakeTableKeyToString(IdKey, LevelKey), IsFind);
	}

	// KMTable_Skill_Normal FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Skill_NormalRow& GetTableKMTable_Skill_Normal(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Skill_NormalRow>(IndexName, IsFind);
	}

	// KMTable_Skill_Normal Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Skill_NormalRow& GetTableKMTable_Skill_NormalByKey(FName IdKey, int32 LevelKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Skill_NormalRow>(*FKMTable_Skill_NormalRow::MakeTableKeyToString(IdKey, LevelKey), IsFind);
	}

	// KMTable_Skill_Projectile FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Skill_ProjectileRow& GetTableKMTable_Skill_Projectile(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Skill_ProjectileRow>(IndexName, IsFind);
	}

	// KMTable_Skill_Projectile Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Skill_ProjectileRow& GetTableKMTable_Skill_ProjectileByKey(FName IdKey, int32 LevelKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Skill_ProjectileRow>(*FKMTable_Skill_ProjectileRow::MakeTableKeyToString(IdKey, LevelKey), IsFind);
	}

	// KMTable_SkillCondition FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillConditionRow& GetTableKMTable_SkillCondition(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillConditionRow>(IndexName, IsFind);
	}

	// KMTable_SkillCondition Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillConditionRow& GetTableKMTable_SkillConditionByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillConditionRow>(*FKMTable_SkillConditionRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_SkillEffect FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillEffectRow& GetTableKMTable_SkillEffect(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillEffectRow>(IndexName, IsFind);
	}

	// KMTable_SkillEffect Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillEffectRow& GetTableKMTable_SkillEffectByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillEffectRow>(*FKMTable_SkillEffectRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_SkillEffectTransition FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillEffectTransitionRow& GetTableKMTable_SkillEffectTransition(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillEffectTransitionRow>(IndexName, IsFind);
	}

	// KMTable_SkillEffectTransition Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillEffectTransitionRow& GetTableKMTable_SkillEffectTransitionByKey(FName IdKey, FName NameKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillEffectTransitionRow>(*FKMTable_SkillEffectTransitionRow::MakeTableKeyToString(IdKey, NameKey), IsFind);
	}

	// KMTable_SkillSet FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillSetRow& GetTableKMTable_SkillSet(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillSetRow>(IndexName, IsFind);
	}

	// KMTable_SkillSet Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_SkillSetRow& GetTableKMTable_SkillSetByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_SkillSetRow>(*FKMTable_SkillSetRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Stage FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StageRow& GetTableKMTable_Stage(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StageRow>(IndexName, IsFind);
	}

	// KMTable_Stage Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StageRow& GetTableKMTable_StageByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StageRow>(*FKMTable_StageRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_StartBattleCharacter FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StartBattleCharacterRow& GetTableKMTable_StartBattleCharacter(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StartBattleCharacterRow>(IndexName, IsFind);
	}

	// KMTable_StartBattleCharacter Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StartBattleCharacterRow& GetTableKMTable_StartBattleCharacterByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StartBattleCharacterRow>(*FKMTable_StartBattleCharacterRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Stat_InLevelUp FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Stat_InLevelUpRow& GetTableKMTable_Stat_InLevelUp(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Stat_InLevelUpRow>(IndexName, IsFind);
	}

	// KMTable_Stat_InLevelUp Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Stat_InLevelUpRow& GetTableKMTable_Stat_InLevelUpByKey(FName IdKey, int32 LevelKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Stat_InLevelUpRow>(*FKMTable_Stat_InLevelUpRow::MakeTableKeyToString(IdKey, LevelKey), IsFind);
	}

	// KMTable_Stat_OutLevelUp FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Stat_OutLevelUpRow& GetTableKMTable_Stat_OutLevelUp(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Stat_OutLevelUpRow>(IndexName, IsFind);
	}

	// KMTable_Stat_OutLevelUp Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_Stat_OutLevelUpRow& GetTableKMTable_Stat_OutLevelUpByKey(FName IdKey, int32 LevelKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_Stat_OutLevelUpRow>(*FKMTable_Stat_OutLevelUpRow::MakeTableKeyToString(IdKey, LevelKey), IsFind);
	}

	// KMTable_StatPerLevel FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StatPerLevelRow& GetTableKMTable_StatPerLevel(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StatPerLevelRow>(IndexName, IsFind);
	}

	// KMTable_StatPerLevel Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_StatPerLevelRow& GetTableKMTable_StatPerLevelByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_StatPerLevelRow>(*FKMTable_StatPerLevelRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_UnlockContent FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_UnlockContentRow& GetTableKMTable_UnlockContent(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_UnlockContentRow>(IndexName, IsFind);
	}

	// KMTable_UnlockContent Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_UnlockContentRow& GetTableKMTable_UnlockContentByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_UnlockContentRow>(*FKMTable_UnlockContentRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_UnlockContentMainLobby FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_UnlockContentMainLobbyRow& GetTableKMTable_UnlockContentMainLobby(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_UnlockContentMainLobbyRow>(IndexName, IsFind);
	}

	// KMTable_UnlockContentMainLobby Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_UnlockContentMainLobbyRow& GetTableKMTable_UnlockContentMainLobbyByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_UnlockContentMainLobbyRow>(*FKMTable_UnlockContentMainLobbyRow::MakeTableKeyToString(IdKey), IsFind);
	}

	// KMTable_Wave FName으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_WaveRow& GetTableKMTable_Wave(FName IndexName, bool& IsFind)
	{
		return *GetTableEM<FKMTable_WaveRow>(IndexName, IsFind);
	}

	// KMTable_Wave Key값으로 검색되는 블루프린트 노출함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FKMTable_WaveRow& GetTableKMTable_WaveByKey(FName IdKey, bool& IsFind)
	{
		return *GetTableEM<FKMTable_WaveRow>(*FKMTable_WaveRow::MakeTableKeyToString(IdKey), IsFind);
	}

};