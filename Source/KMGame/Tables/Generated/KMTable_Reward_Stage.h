#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Reward.h"
#include "KMTable_Reward_Stage.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Reward_StageRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Reward_StageRow : public FKMTable_RewardRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Reward_StageRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Reward_StageRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 39;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Reward_Stage Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Reward_StageRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Reward_StageRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Reward_StageRow* FKMTable_Reward_StageRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Reward_StageRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Reward_StageRow& FKMTable_Reward_StageRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

