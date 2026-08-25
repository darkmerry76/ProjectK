#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative.h"
#include "KMTable_Narrative_Prologue.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Narrative_PrologueRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Narrative_PrologueRow : public FKMTable_NarrativeRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                StartDelay = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                EndDelay = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Duration = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                FadeInTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                FadeOutTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Branch = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Narrative_PrologueRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Narrative_PrologueRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 34;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative_Prologue Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Narrative_PrologueRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Narrative_PrologueRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Narrative_PrologueRow* FKMTable_Narrative_PrologueRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Narrative_PrologueRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Narrative_PrologueRow& FKMTable_Narrative_PrologueRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

