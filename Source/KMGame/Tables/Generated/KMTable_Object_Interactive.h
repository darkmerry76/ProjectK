#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object.h"
#include "KMTable_Object_Interactive.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Object_InteractiveRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Object_InteractiveRow : public FKMTable_ObjectRow
{
	GENERATED_USTRUCT_BODY()

	// PDA
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   AssetPda = { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Object_InteractiveRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Object_InteractiveRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 36;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object_Interactive Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Object_InteractiveRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Object_InteractiveRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Object_InteractiveRow* FKMTable_Object_InteractiveRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Object_InteractiveRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Object_InteractiveRow& FKMTable_Object_InteractiveRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

