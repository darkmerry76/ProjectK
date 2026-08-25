#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_NarrativeRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_NarrativeRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   NodePda = { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_NarrativeRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_NarrativeRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 29;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_NarrativeRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_NarrativeRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_NarrativeRow* FKMTable_NarrativeRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_NarrativeRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_NarrativeRow& FKMTable_NarrativeRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

