#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_BaseStat.h"
#include "KMTable_BaseStat_Monster.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BaseStat_MonsterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BaseStat_MonsterRow : public FKMTable_BaseStatRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BaseStat_MonsterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BaseStat_MonsterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 6;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_BaseStat_Monster Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BaseStat_MonsterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BaseStat_MonsterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BaseStat_MonsterRow* FKMTable_BaseStat_MonsterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BaseStat_MonsterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BaseStat_MonsterRow& FKMTable_BaseStat_MonsterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

