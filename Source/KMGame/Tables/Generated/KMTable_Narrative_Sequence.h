#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative.h"
#include "KMTable_Narrative_Sequence.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Narrative_SequenceRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Narrative_SequenceRow : public FKMTable_NarrativeRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        task;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Narrative_SequenceRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Narrative_SequenceRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 31;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative_Sequence Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Narrative_SequenceRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Narrative_SequenceRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Narrative_SequenceRow* FKMTable_Narrative_SequenceRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Narrative_SequenceRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Narrative_SequenceRow& FKMTable_Narrative_SequenceRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

