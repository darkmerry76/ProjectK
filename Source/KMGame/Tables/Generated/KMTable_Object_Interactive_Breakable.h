#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object_Interactive.h"
#include "KMTable_Object_Interactive_Breakable.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Object_Interactive_BreakableRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Object_Interactive_BreakableRow : public FKMTable_Object_InteractiveRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Object_Interactive_BreakableRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Object_Interactive_BreakableRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 37;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object_Interactive_Breakable Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Object_Interactive_BreakableRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Object_Interactive_BreakableRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Object_Interactive_BreakableRow* FKMTable_Object_Interactive_BreakableRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Object_Interactive_BreakableRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Object_Interactive_BreakableRow& FKMTable_Object_Interactive_BreakableRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

