#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_InGameEvent.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_InGameEventRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_InGameEventRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 게임이벤트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 기획메모
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 IsShowPopupMsg = { false };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_InGameEventRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_InGameEventRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 17;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_InGameEvent Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_InGameEventRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_InGameEventRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_InGameEventRow* FKMTable_InGameEventRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_InGameEventRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_InGameEventRow& FKMTable_InGameEventRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

