#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Character_DropRate.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Character_DropRateRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Character_DropRateRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 아이디
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                id = { NAME_None };

	// 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                characterId = { NAME_None };

	// 캐릭터 확률 테이블 아이디
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                characterDropRate = { 0.f };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Character_DropRateRow* FindRowPtr(FName idKey, FName characterIdKey);
	inline static const FKMTable_Character_DropRateRow& FindRow(FName idKey, FName characterIdKey);
	inline static FString MakeTableKeyToString(FName idKey, FName characterIdKey);

	static const int32   RefTableIndex = 11;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Character_DropRate Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Character_DropRateRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Character_DropRateRow::MakeTableKeyToString(FName idKey, FName characterIdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(idKey, characterIdKey);
}

inline const FKMTable_Character_DropRateRow* FKMTable_Character_DropRateRow::FindRowPtr(FName idKey, FName characterIdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Character_DropRateRow>(*MakeTableKeyToString(idKey, characterIdKey));
}

inline const FKMTable_Character_DropRateRow& FKMTable_Character_DropRateRow::FindRow(FName idKey, FName characterIdKey)
{
	return *FindRowPtr(idKey, characterIdKey);
}

