#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillEffectTransition.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillEffectTransitionRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillEffectTransitionRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 게임플레이 Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Name = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                BranchEffectId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                Priority = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillEffectTransitionRow* FindRowPtr(FName IdKey, FName NameKey);
	inline static const FKMTable_SkillEffectTransitionRow& FindRow(FName IdKey, FName NameKey);
	inline static FString MakeTableKeyToString(FName IdKey, FName NameKey);

	static const int32   RefTableIndex = 48;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillEffectTransition Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillEffectTransitionRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillEffectTransitionRow::MakeTableKeyToString(FName IdKey, FName NameKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, NameKey);
}

inline const FKMTable_SkillEffectTransitionRow* FKMTable_SkillEffectTransitionRow::FindRowPtr(FName IdKey, FName NameKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillEffectTransitionRow>(*MakeTableKeyToString(IdKey, NameKey));
}

inline const FKMTable_SkillEffectTransitionRow& FKMTable_SkillEffectTransitionRow::FindRow(FName IdKey, FName NameKey)
{
	return *FindRowPtr(IdKey, NameKey);
}

