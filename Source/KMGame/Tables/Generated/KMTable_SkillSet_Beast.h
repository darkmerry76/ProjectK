#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillSet.h"
#include "KMTable_SkillSet_Beast.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillSet_BeastRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillSet_BeastRow : public FKMTable_SkillSetRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillSet_BeastRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillSet_BeastRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 50;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillSet_Beast Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillSet_BeastRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillSet_BeastRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillSet_BeastRow* FKMTable_SkillSet_BeastRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillSet_BeastRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillSet_BeastRow& FKMTable_SkillSet_BeastRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

