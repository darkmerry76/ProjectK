#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Item.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_ItemRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_ItemRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                ItemPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_ItemRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_ItemRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 20;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Item Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_ItemRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_ItemRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_ItemRow* FKMTable_ItemRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_ItemRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_ItemRow& FKMTable_ItemRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

