#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Character.h"
#include "KMTable_Character_Hero.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Character_HeroRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Character_HeroRow : public FKMTable_CharacterRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 전체 컨셉이나 스토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Description;

	// 각성 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              AwakeDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMFightStyleType    FightStyle = { EKMFightStyleType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCoreArmamentType  CoreArmament = { EKMCoreArmamentType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterBattleRole role = { EKMCharacterBattleRole::None };

	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMHeroGrade         Grade = { EKMHeroGrade::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                IconPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Character_HeroRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Character_HeroRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 11;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Character_Hero Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Character_HeroRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Character_HeroRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Character_HeroRow* FKMTable_Character_HeroRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Character_HeroRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Character_HeroRow& FKMTable_Character_HeroRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

