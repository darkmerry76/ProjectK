#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_UnlockContent.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_UnlockContentRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_UnlockContentRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 언락 콘텐츠 Id
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                Name = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                UnlockConditionPlayerLevel = { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                UnlockConditionStageId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                UnlockConditionChapterId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                ButtonIconPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_UnlockContentRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_UnlockContentRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 62;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_UnlockContent Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_UnlockContentRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_UnlockContentRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_UnlockContentRow* FKMTable_UnlockContentRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_UnlockContentRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_UnlockContentRow& FKMTable_UnlockContentRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

