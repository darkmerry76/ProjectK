#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative_Task.h"
#include "KMTable_Narrative_Task_Dialog.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Narrative_Task_DialogRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Narrative_Task_DialogRow : public FKMTable_Narrative_TaskRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Narrative_Task_DialogRow* FindRowPtr(FName IdKey, int32 subIdKey);
	inline static const FKMTable_Narrative_Task_DialogRow& FindRow(FName IdKey, int32 subIdKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 subIdKey);

	static const int32   RefTableIndex = 30;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative_Task_Dialog Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Narrative_Task_DialogRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Narrative_Task_DialogRow::MakeTableKeyToString(FName IdKey, int32 subIdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, subIdKey);
}

inline const FKMTable_Narrative_Task_DialogRow* FKMTable_Narrative_Task_DialogRow::FindRowPtr(FName IdKey, int32 subIdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Narrative_Task_DialogRow>(*MakeTableKeyToString(IdKey, subIdKey));
}

inline const FKMTable_Narrative_Task_DialogRow& FKMTable_Narrative_Task_DialogRow::FindRow(FName IdKey, int32 subIdKey)
{
	return *FindRowPtr(IdKey, subIdKey);
}

