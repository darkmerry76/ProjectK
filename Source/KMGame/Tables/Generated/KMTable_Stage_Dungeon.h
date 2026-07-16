#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Stage.h"
#include "KMTable_Stage_Dungeon.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Stage_DungeonRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Stage_DungeonRow : public FKMTable_StageRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Stage_DungeonRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Stage_DungeonRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 52;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Stage_Dungeon Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Stage_DungeonRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Stage_DungeonRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Stage_DungeonRow* FKMTable_Stage_DungeonRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Stage_DungeonRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Stage_DungeonRow& FKMTable_Stage_DungeonRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

