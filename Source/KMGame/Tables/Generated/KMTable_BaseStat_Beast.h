#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_BaseStat.h"
#include "KMTable_BaseStat_Beast.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BaseStat_BeastRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BaseStat_BeastRow : public FKMTable_BaseStatRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BaseStat_BeastRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BaseStat_BeastRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 3;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_BaseStat_Beast Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BaseStat_BeastRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BaseStat_BeastRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BaseStat_BeastRow* FKMTable_BaseStat_BeastRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BaseStat_BeastRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BaseStat_BeastRow& FKMTable_BaseStat_BeastRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

