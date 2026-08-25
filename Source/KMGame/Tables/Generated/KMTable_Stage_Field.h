#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Stage.h"
#include "KMTable_Stage_Field.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Stage_FieldRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Stage_FieldRow : public FKMTable_StageRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Stage_FieldRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Stage_FieldRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 56;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Stage_Field Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Stage_FieldRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Stage_FieldRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Stage_FieldRow* FKMTable_Stage_FieldRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Stage_FieldRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Stage_FieldRow& FKMTable_Stage_FieldRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

