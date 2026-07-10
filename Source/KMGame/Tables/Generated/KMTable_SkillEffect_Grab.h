#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillEffect.h"
#include "KMTable_SkillEffect_Grab.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillEffect_GrabRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillEffect_GrabRow : public FKMTable_SkillEffectRow
{
	GENERATED_USTRUCT_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillEffect_GrabRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillEffect_GrabRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 44;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillEffect_Grab Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillEffect_GrabRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillEffect_GrabRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillEffect_GrabRow* FKMTable_SkillEffect_GrabRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillEffect_GrabRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillEffect_GrabRow& FKMTable_SkillEffect_GrabRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

