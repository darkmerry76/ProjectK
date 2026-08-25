#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillEffect.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillEffectRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillEffectRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Name = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                category = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                ActivatedEvent = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillEffectType   Type = { EKMSkillEffectType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillEffectTargetType TargetType = { EKMSkillEffectTargetType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMStatParameterType Parameter = { EKMStatParameterType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMStatParameterType ScaleType = { EKMStatParameterType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMOperatorType      Operator = { EKMOperatorType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   Ability = { };

	UPROPERTY(EditAnywhere)
	double               BaseValue = { 0.f };

	UPROPERTY(EditAnywhere)
	double               SecondaryValue = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Duration = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                Interval = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMDamagePowerType   PowerEventType = { EKMDamagePowerType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                OverlapGroup = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        ClearGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillEffectOverlapType OverlapType = { EKMSkillEffectOverlapType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                OverlapCount = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransitionId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        ReadGameplaytag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        WriteGameplayTag;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillEffectRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillEffectRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 47;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillEffect Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillEffectRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillEffectRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillEffectRow* FKMTable_SkillEffectRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillEffectRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillEffectRow& FKMTable_SkillEffectRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

