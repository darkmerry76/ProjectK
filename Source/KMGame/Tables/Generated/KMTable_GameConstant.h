#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_GameConstant.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_GameConstantRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_GameConstantRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                ConstantValue = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_GameConstantRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_GameConstantRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 17;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_GameConstant Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_GameConstantRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_GameConstantRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_GameConstantRow* FKMTable_GameConstantRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_GameConstantRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_GameConstantRow& FKMTable_GameConstantRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

