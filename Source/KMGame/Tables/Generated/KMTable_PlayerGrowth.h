#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_PlayerGrowth.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_PlayerGrowthRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_PlayerGrowthRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 성장
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                RequirePlayerLevel = { 0 };

	// 성장 버프 스킬
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                UnlockSkillName = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                ApplySkillLevel = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                RequireGoldCost = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_PlayerGrowthRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_PlayerGrowthRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 34;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_PlayerGrowth Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_PlayerGrowthRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_PlayerGrowthRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_PlayerGrowthRow* FKMTable_PlayerGrowthRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_PlayerGrowthRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_PlayerGrowthRow& FKMTable_PlayerGrowthRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

