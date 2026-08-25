#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_LocalizeText.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_LocalizeTextRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_LocalizeTextRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Text_Ko;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Text_En;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_LocalizeTextRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_LocalizeTextRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 19;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_LocalizeText Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_LocalizeTextRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_LocalizeTextRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_LocalizeTextRow* FKMTable_LocalizeTextRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_LocalizeTextRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_LocalizeTextRow& FKMTable_LocalizeTextRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

