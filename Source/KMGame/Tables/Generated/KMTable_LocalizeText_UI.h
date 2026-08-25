#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_LocalizeText.h"
#include "KMTable_LocalizeText_UI.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_LocalizeText_UIRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_LocalizeText_UIRow : public FKMTable_LocalizeTextRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_LocalizeText_UIRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_LocalizeText_UIRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 23;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_LocalizeText_UI Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_LocalizeText_UIRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_LocalizeText_UIRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_LocalizeText_UIRow* FKMTable_LocalizeText_UIRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_LocalizeText_UIRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_LocalizeText_UIRow& FKMTable_LocalizeText_UIRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

