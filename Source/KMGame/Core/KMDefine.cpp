// Fill out your copyright notice in the Description page of Project Settings.


#include "KMDefine.h"
#include "Tables/Generated/KMTable_Skill.h"

FKMSkillKey::FKMSkillKey()
{
	
}

FKMSkillKey::FKMSkillKey(FName tableId, int32 level) 
{
	TableId = tableId;
	Level = level;
	if (TableId != NAME_None)
	{
		TableRecord = FKMTable_SkillRow::FindRowPtr(TableId, Level);
		check(TableRecord != nullptr);
	}
}

FKMSkillKey::FKMSkillKey(const FKMSkillKeyBase& otherKey) 
{
	TableId = otherKey.TableId;
	Level = otherKey.Level;
	TableRecord = otherKey.TableRecord;
}

namespace KMGame
{

UEnum* GetCharacterTypeEnum()
{
	static UEnum* resultEnum = FindObject<UEnum>(nullptr, TEXT("/Script/KMGame.EKMCharacterType"));
	return resultEnum;
}
	
}