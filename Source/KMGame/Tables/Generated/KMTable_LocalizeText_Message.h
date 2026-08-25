#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_LocalizeText.h"
#include "KMTable_LocalizeText_Message.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_LocalizeText_MessageRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_LocalizeText_MessageRow : public FKMTable_LocalizeTextRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_LocalizeText_MessageRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_LocalizeText_MessageRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 27;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_LocalizeText_Message Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_LocalizeText_MessageRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_LocalizeText_MessageRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_LocalizeText_MessageRow* FKMTable_LocalizeText_MessageRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_LocalizeText_MessageRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_LocalizeText_MessageRow& FKMTable_LocalizeText_MessageRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

