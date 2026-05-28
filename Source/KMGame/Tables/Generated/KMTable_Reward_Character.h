#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Reward.h"
#include "KMTable_Reward_Character.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Reward_CharacterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Reward_CharacterRow : public FKMTable_RewardRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Reward_CharacterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Reward_CharacterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 36;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Reward_Character Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Reward_CharacterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Reward_CharacterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Reward_CharacterRow* FKMTable_Reward_CharacterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Reward_CharacterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Reward_CharacterRow& FKMTable_Reward_CharacterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

