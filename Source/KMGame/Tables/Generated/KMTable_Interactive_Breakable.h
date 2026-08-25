#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Interactive.h"
#include "KMTable_Interactive_Breakable.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Interactive_BreakableRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Interactive_BreakableRow : public FKMTable_InteractiveRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Interactive_BreakableRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Interactive_BreakableRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 22;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Interactive_Breakable Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Interactive_BreakableRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Interactive_BreakableRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Interactive_BreakableRow* FKMTable_Interactive_BreakableRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Interactive_BreakableRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Interactive_BreakableRow& FKMTable_Interactive_BreakableRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

