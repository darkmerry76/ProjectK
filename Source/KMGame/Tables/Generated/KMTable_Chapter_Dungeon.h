#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Chapter.h"
#include "KMTable_Chapter_Dungeon.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Chapter_DungeonRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Chapter_DungeonRow : public FKMTable_ChapterRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Chapter_DungeonRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Chapter_DungeonRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 7;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Chapter_Dungeon Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Chapter_DungeonRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Chapter_DungeonRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Chapter_DungeonRow* FKMTable_Chapter_DungeonRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Chapter_DungeonRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Chapter_DungeonRow& FKMTable_Chapter_DungeonRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

