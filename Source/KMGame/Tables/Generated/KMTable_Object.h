#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Object.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_ObjectRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_ObjectRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              FullName;

	// 캐릭터 전체 컨셉이나 스토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMObjectType        Type = { EKMObjectType::Hero };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 IsCarryable = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                scale = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                StatId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                IconPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_ObjectRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_ObjectRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 31;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Object Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_ObjectRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_ObjectRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_ObjectRow* FKMTable_ObjectRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_ObjectRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_ObjectRow& FKMTable_ObjectRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

