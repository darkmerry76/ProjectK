#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_OutLevelupCost.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_OutLevelupCostRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_OutLevelupCostRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 레벨 (2부터 소비되는 재화)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32                Id = { 0 };

	// 레벨별 소모 금화
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                LevelUpCostGold = { 0 };

	// 레벨업 소모 아이템이 있는 경우 (단수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                LevelUpCostItemId = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_OutLevelupCostRow* FindRowPtr(int32 IdKey);
	inline static const FKMTable_OutLevelupCostRow& FindRow(int32 IdKey);
	inline static FString MakeTableKeyToString(int32 IdKey);

	static const int32   RefTableIndex = 32;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_OutLevelupCost Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_OutLevelupCostRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_OutLevelupCostRow::MakeTableKeyToString(int32 IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_OutLevelupCostRow* FKMTable_OutLevelupCostRow::FindRowPtr(int32 IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_OutLevelupCostRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_OutLevelupCostRow& FKMTable_OutLevelupCostRow::FindRow(int32 IdKey)
{
	return *FindRowPtr(IdKey);
}

