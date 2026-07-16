#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Narrative.h"
#include "KMTable_Narrative_Dialog.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Narrative_DialogRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Narrative_DialogRow : public FKMTable_NarrativeRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32                subId = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 MessageClear = { false };

	// 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor               MessageColor = { 255, 255, 255, 255 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Speaker = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   CinematicWidget = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                VoiceId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   Facial = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Animation = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                FadeInTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Duration = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                FadeOutTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Branch = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Narrative_DialogRow* FindRowPtr(FName IdKey, int32 subIdKey);
	inline static const FKMTable_Narrative_DialogRow& FindRow(FName IdKey, int32 subIdKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 subIdKey);

	static const int32   RefTableIndex = 28;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Narrative_Dialog Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Narrative_DialogRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Narrative_DialogRow::MakeTableKeyToString(FName IdKey, int32 subIdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, subIdKey);
}

inline const FKMTable_Narrative_DialogRow* FKMTable_Narrative_DialogRow::FindRowPtr(FName IdKey, int32 subIdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Narrative_DialogRow>(*MakeTableKeyToString(IdKey, subIdKey));
}

inline const FKMTable_Narrative_DialogRow& FKMTable_Narrative_DialogRow::FindRow(FName IdKey, int32 subIdKey)
{
	return *FindRowPtr(IdKey, subIdKey);
}

