#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_StartBattleCharacter.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_StartBattleCharacterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_StartBattleCharacterRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 텍스쳐 Id
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        BattleDeckList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_StartBattleCharacterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_StartBattleCharacterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 54;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_StartBattleCharacter Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_StartBattleCharacterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_StartBattleCharacterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_StartBattleCharacterRow* FKMTable_StartBattleCharacterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_StartBattleCharacterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_StartBattleCharacterRow& FKMTable_StartBattleCharacterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

