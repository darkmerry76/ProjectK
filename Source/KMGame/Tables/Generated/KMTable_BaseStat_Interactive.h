#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_BaseStat.h"
#include "KMTable_BaseStat_Interactive.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BaseStat_InteractiveRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BaseStat_InteractiveRow : public FKMTable_BaseStatRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BaseStat_InteractiveRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BaseStat_InteractiveRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 5;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_BaseStat_Interactive Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BaseStat_InteractiveRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BaseStat_InteractiveRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BaseStat_InteractiveRow* FKMTable_BaseStat_InteractiveRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BaseStat_InteractiveRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BaseStat_InteractiveRow& FKMTable_BaseStat_InteractiveRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

