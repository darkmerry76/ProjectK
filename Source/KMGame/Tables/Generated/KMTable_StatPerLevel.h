#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_StatPerLevel.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_StatPerLevelRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_StatPerLevelRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 레벨별 체력 증가율 (100분율)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                HealthPointPerLevel = { 0 };

	// 레벨별 공격력 증가율 (100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                AttackPointPerLevel = { 0 };

	// 최대 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MaxLevel = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_StatPerLevelRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_StatPerLevelRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 56;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_StatPerLevel Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_StatPerLevelRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_StatPerLevelRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_StatPerLevelRow* FKMTable_StatPerLevelRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_StatPerLevelRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_StatPerLevelRow& FKMTable_StatPerLevelRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

