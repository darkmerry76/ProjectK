#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Drop.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_DropRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_DropRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// Reward ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                DropRate = { 0.f };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_DropRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_DropRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 13;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Drop Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_DropRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_DropRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_DropRow* FKMTable_DropRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_DropRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_DropRow& FKMTable_DropRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

