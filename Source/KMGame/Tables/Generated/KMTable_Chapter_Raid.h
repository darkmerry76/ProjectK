#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Chapter.h"
#include "KMTable_Chapter_Raid.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Chapter_RaidRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Chapter_RaidRow : public FKMTable_ChapterRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Chapter_RaidRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Chapter_RaidRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 9;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Chapter_Raid Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Chapter_RaidRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Chapter_RaidRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Chapter_RaidRow* FKMTable_Chapter_RaidRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Chapter_RaidRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Chapter_RaidRow& FKMTable_Chapter_RaidRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

