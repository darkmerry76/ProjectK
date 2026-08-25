#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillEffect.h"
#include "KMTable_SkillEffect_Normal.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillEffect_NormalRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillEffect_NormalRow : public FKMTable_SkillEffectRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillEffect_NormalRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillEffect_NormalRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 49;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillEffect_Normal Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillEffect_NormalRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillEffect_NormalRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillEffect_NormalRow* FKMTable_SkillEffect_NormalRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillEffect_NormalRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillEffect_NormalRow& FKMTable_SkillEffect_NormalRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

