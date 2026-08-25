#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object.h"
#include "KMTable_Object_Character.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Object_CharacterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Object_CharacterRow : public FKMTable_ObjectRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                DefaultBeast = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   pdaKey = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        DefaultSkillSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMLocationType      LocationType = { EKMLocationType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransformSkill = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Object_CharacterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Object_CharacterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 33;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object_Character Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Object_CharacterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Object_CharacterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Object_CharacterRow* FKMTable_Object_CharacterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Object_CharacterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Object_CharacterRow& FKMTable_Object_CharacterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

