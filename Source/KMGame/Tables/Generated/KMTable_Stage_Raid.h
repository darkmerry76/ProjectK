#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Stage.h"
#include "KMTable_Stage_Raid.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Stage_RaidRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Stage_RaidRow : public FKMTable_StageRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Stage_RaidRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Stage_RaidRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 55;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Stage_Raid Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Stage_RaidRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Stage_RaidRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Stage_RaidRow* FKMTable_Stage_RaidRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Stage_RaidRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Stage_RaidRow& FKMTable_Stage_RaidRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

