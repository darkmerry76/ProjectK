#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative.h"
#include "KMTable_Narrative_Event.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Narrative_EventRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Narrative_EventRow : public FKMTable_NarrativeRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   Asset = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                EventTag = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Branch = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Narrative_EventRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Narrative_EventRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 30;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative_Event Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Narrative_EventRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Narrative_EventRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Narrative_EventRow* FKMTable_Narrative_EventRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Narrative_EventRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Narrative_EventRow& FKMTable_Narrative_EventRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

