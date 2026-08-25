#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Reward.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_RewardRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_RewardRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// Reward ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                Exp = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMOperatorType      ExpOperator = { EKMOperatorType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                Gold = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMOperatorType      GoldOperator = { EKMOperatorType::None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_RewardRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_RewardRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 42;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Reward Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_RewardRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_RewardRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_RewardRow* FKMTable_RewardRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_RewardRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_RewardRow& FKMTable_RewardRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

