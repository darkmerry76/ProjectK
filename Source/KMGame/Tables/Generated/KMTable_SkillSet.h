#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillSet.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillSetRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillSetRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 Default = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 Grab = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 Special = { false };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillSetRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillSetRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 48;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillSet Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillSetRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillSetRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillSetRow* FKMTable_SkillSetRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillSetRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillSetRow& FKMTable_SkillSetRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

