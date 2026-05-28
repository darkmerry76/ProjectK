#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_BaseStat.h"
#include "KMTable_BaseStat_Hero.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BaseStat_HeroRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BaseStat_HeroRow : public FKMTable_BaseStatRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BaseStat_HeroRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BaseStat_HeroRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 4;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_BaseStat_Hero Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BaseStat_HeroRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BaseStat_HeroRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BaseStat_HeroRow* FKMTable_BaseStat_HeroRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BaseStat_HeroRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BaseStat_HeroRow& FKMTable_BaseStat_HeroRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

