#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Skill.h"
#include "KMTable_Skill_Projectile.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Skill_ProjectileRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Skill_ProjectileRow : public FKMTable_SkillRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TriggerEvent = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransformSocket = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                AnimTime = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   ProjectileBp = { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Skill_ProjectileRow* FindRowPtr(FName IdKey, int32 LevelKey);
	inline static const FKMTable_Skill_ProjectileRow& FindRow(FName IdKey, int32 LevelKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 LevelKey);

	static const int32   RefTableIndex = 43;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Skill_Projectile Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Skill_ProjectileRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Skill_ProjectileRow::MakeTableKeyToString(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, LevelKey);
}

inline const FKMTable_Skill_ProjectileRow* FKMTable_Skill_ProjectileRow::FindRowPtr(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Skill_ProjectileRow>(*MakeTableKeyToString(IdKey, LevelKey));
}

inline const FKMTable_Skill_ProjectileRow& FKMTable_Skill_ProjectileRow::FindRow(FName IdKey, int32 LevelKey)
{
	return *FindRowPtr(IdKey, LevelKey);
}

