#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Chapter.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_ChapterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_ChapterRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Map = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                SpawnTable = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        WaveIdList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_ChapterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_ChapterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 7;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Chapter Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_ChapterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_ChapterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_ChapterRow* FKMTable_ChapterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_ChapterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_ChapterRow& FKMTable_ChapterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

