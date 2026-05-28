#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillSet.h"
#include "KMTable_SkillSet_Hero.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillSet_HeroRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillSet_HeroRow : public FKMTable_SkillSetRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillSet_HeroRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillSet_HeroRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 47;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillSet_Hero Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillSet_HeroRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillSet_HeroRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillSet_HeroRow* FKMTable_SkillSet_HeroRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillSet_HeroRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillSet_HeroRow& FKMTable_SkillSet_HeroRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

