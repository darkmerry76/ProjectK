#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Wave.h"
#include "KMTable_Wave_Field.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Wave_FieldRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Wave_FieldRow : public FKMTable_WaveRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Wave_FieldRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Wave_FieldRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 68;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Wave_Field Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Wave_FieldRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Wave_FieldRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Wave_FieldRow* FKMTable_Wave_FieldRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Wave_FieldRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Wave_FieldRow& FKMTable_Wave_FieldRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

