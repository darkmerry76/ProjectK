#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Drop.h"
#include "KMTable_Drop_InGameSkill.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Drop_InGameSkillRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Drop_InGameSkillRow : public FKMTable_DropRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                DropInGameSkillId = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Drop_InGameSkillRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_Drop_InGameSkillRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 13;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Drop_InGameSkill Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Drop_InGameSkillRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Drop_InGameSkillRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_Drop_InGameSkillRow* FKMTable_Drop_InGameSkillRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Drop_InGameSkillRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_Drop_InGameSkillRow& FKMTable_Drop_InGameSkillRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

