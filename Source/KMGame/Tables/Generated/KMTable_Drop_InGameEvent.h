#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Drop.h"
#include "KMTable_Drop_InGameEvent.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Drop_InGameEventRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Drop_InGameEventRow : public FKMTable_DropRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                InGameEventId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TriggerType = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Drop_InGameEventRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Drop_InGameEventRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 12;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Drop_InGameEvent Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Drop_InGameEventRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Drop_InGameEventRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Drop_InGameEventRow* FKMTable_Drop_InGameEventRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Drop_InGameEventRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Drop_InGameEventRow& FKMTable_Drop_InGameEventRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

