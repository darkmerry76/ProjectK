#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Wave.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_WaveRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_WaveRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 등장 보스 몬스터Id 리스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        BossMonsterIdList;

	// 등장가능한 최대 몬스터 수 (보스는 제외)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                SpawnableMonsterMaxCount = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                CharacterDropRateId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        Reward;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_WaveRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_WaveRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 60;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Wave Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_WaveRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_WaveRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_WaveRow* FKMTable_WaveRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_WaveRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_WaveRow& FKMTable_WaveRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

