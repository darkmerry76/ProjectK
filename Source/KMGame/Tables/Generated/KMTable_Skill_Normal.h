#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Skill.h"
#include "KMTable_Skill_Normal.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Skill_NormalRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Skill_NormalRow : public FKMTable_SkillRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillScopeType    ScopeType = { EKMSkillScopeType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                SkillCondition = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MaxScopeCount = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                CoolTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Duration = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                EndTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   Ability = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   Bp = { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Skill_NormalRow* FindRowPtr(FName IdKey, int32 LevelKey);
	inline static const FKMTable_Skill_NormalRow& FindRow(FName IdKey, int32 LevelKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 LevelKey);

	static const int32   RefTableIndex = 42;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Skill_Normal Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Skill_NormalRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Skill_NormalRow::MakeTableKeyToString(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, LevelKey);
}

inline const FKMTable_Skill_NormalRow* FKMTable_Skill_NormalRow::FindRowPtr(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Skill_NormalRow>(*MakeTableKeyToString(IdKey, LevelKey));
}

inline const FKMTable_Skill_NormalRow& FKMTable_Skill_NormalRow::FindRow(FName IdKey, int32 LevelKey)
{
	return *FindRowPtr(IdKey, LevelKey);
}

