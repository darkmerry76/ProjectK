#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_LocalizeText.h"
#include "KMTable_LocalizeText_Format.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_LocalizeText_FormatRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_LocalizeText_FormatRow : public FKMTable_LocalizeTextRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_LocalizeText_FormatRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_LocalizeText_FormatRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 23;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_LocalizeText_Format Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_LocalizeText_FormatRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_LocalizeText_FormatRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_LocalizeText_FormatRow* FKMTable_LocalizeText_FormatRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_LocalizeText_FormatRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_LocalizeText_FormatRow& FKMTable_LocalizeText_FormatRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

