#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object.h"
#include "KMTable_Object_Beast.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Object_BeastRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Object_BeastRow : public FKMTable_ObjectRow
{
	GENERATED_USTRUCT_BODY()

	// 각성 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              AwakeDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   AssetPda = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMBattleRoleType    role = { EKMBattleRoleType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMLocationType      LocationType = { EKMLocationType::None };

	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMHeroGradeType     Grade = { EKMHeroGradeType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransformSkill = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Object_BeastRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Object_BeastRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 32;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object_Beast Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Object_BeastRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Object_BeastRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Object_BeastRow* FKMTable_Object_BeastRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Object_BeastRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Object_BeastRow& FKMTable_Object_BeastRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

