#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_LocalizeText.h"
#include "KMTable_LocalizeText_Desc.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_LocalizeText_DescRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_LocalizeText_DescRow : public FKMTable_LocalizeTextRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_LocalizeText_DescRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_LocalizeText_DescRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 22;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_LocalizeText_Desc Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_LocalizeText_DescRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_LocalizeText_DescRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_LocalizeText_DescRow* FKMTable_LocalizeText_DescRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_LocalizeText_DescRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_LocalizeText_DescRow& FKMTable_LocalizeText_DescRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

