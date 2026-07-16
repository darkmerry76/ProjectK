#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Drop.h"
#include "KMTable_Drop_Item.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Drop_ItemRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Drop_ItemRow : public FKMTable_DropRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                DropItemId = { NAME_None };

	// 드랍이 발동하면 최소~최대 사이에서 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>        DropStackCount;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Drop_ItemRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Drop_ItemRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 17;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Drop_Item Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Drop_ItemRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Drop_ItemRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Drop_ItemRow* FKMTable_Drop_ItemRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Drop_ItemRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Drop_ItemRow& FKMTable_Drop_ItemRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

