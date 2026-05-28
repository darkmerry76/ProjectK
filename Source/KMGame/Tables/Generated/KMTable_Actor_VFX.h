#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Actor.h"
#include "KMTable_Actor_VFX.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Actor_VFXRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Actor_VFXRow : public FKMTable_ActorRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Actor_VFXRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Actor_VFXRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Actor_VFX Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Actor_VFXRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Actor_VFXRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Actor_VFXRow* FKMTable_Actor_VFXRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Actor_VFXRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Actor_VFXRow& FKMTable_Actor_VFXRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

