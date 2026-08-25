#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Interactive.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_InteractiveRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_InteractiveRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	// 자세한이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              FullName;

	// 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Description;

	// PDA
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   AssetPda = { };

	// 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                scale = { 0.f };

	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMHeroGrade         Grade = { EKMHeroGrade::None };

	// 스텟
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                StatId = { NAME_None };

	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                IconPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_InteractiveRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_InteractiveRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 21;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Interactive Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_InteractiveRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_InteractiveRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_InteractiveRow* FKMTable_InteractiveRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_InteractiveRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_InteractiveRow& FKMTable_InteractiveRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

