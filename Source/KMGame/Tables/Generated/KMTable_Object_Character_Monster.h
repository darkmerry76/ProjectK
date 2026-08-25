#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object_Character.h"
#include "KMTable_Object_Character_Monster.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Object_Character_MonsterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Object_Character_MonsterRow : public FKMTable_Object_CharacterRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMMonsterGradeType  MonsterGrade = { EKMMonsterGradeType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                DefaultLevel = { 0 };

	// 인게임레벨업 필요
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 NeedInLvUp = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                purify_rate = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        InitSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>        SealCount;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Object_Character_MonsterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Object_Character_MonsterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 35;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object_Character_Monster Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Object_Character_MonsterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Object_Character_MonsterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Object_Character_MonsterRow* FKMTable_Object_Character_MonsterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Object_Character_MonsterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Object_Character_MonsterRow& FKMTable_Object_Character_MonsterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

