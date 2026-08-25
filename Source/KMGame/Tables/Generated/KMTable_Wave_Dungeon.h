#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Wave.h"
#include "KMTable_Wave_Dungeon.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Wave_DungeonRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Wave_DungeonRow : public FKMTable_WaveRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Wave_DungeonRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Wave_DungeonRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 65;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Wave_Dungeon Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Wave_DungeonRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Wave_DungeonRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Wave_DungeonRow* FKMTable_Wave_DungeonRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Wave_DungeonRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Wave_DungeonRow& FKMTable_Wave_DungeonRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

