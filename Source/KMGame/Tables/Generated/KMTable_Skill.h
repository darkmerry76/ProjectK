#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Skill.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32                Level = { 0 };

	// 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Category = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillType         Type = { EKMSkillType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillActiveType   ActiveType = { EKMSkillActiveType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EKMSkillTargetType> TargetType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                OverlapCount = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Range = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                UnlockLevel = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                UnlockGrade = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        UnlockSkill;

	UPROPERTY(EditAnywhere)
	double               CostHp = { 0.f };

	UPROPERTY(EditAnywhere)
	double               CostMp = { 0.f };

	UPROPERTY(EditAnywhere)
	double               CostEmp = { 0.f };

	UPROPERTY(EditAnywhere)
	double               CostTempo = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                AnimTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        GameplayTag;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillRow* FindRowPtr(FName IdKey, int32 LevelKey);
	inline static const FKMTable_SkillRow& FindRow(FName IdKey, int32 LevelKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 LevelKey);

	static const int32   RefTableIndex = 40;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Skill Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillRow::MakeTableKeyToString(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, LevelKey);
}

inline const FKMTable_SkillRow* FKMTable_SkillRow::FindRowPtr(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillRow>(*MakeTableKeyToString(IdKey, LevelKey));
}

inline const FKMTable_SkillRow& FKMTable_SkillRow::FindRow(FName IdKey, int32 LevelKey)
{
	return *FindRowPtr(IdKey, LevelKey);
}

