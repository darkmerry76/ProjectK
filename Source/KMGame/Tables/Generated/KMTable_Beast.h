#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Beast.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BeastRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BeastRow : public FEM_TableBaseRow
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

	// 각성 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              AwakeDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterType     Type = { EKMCharacterType::Hero };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   AssetPda = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterBattleRole role = { EKMCharacterBattleRole::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterLocationType LocationType = { EKMCharacterLocationType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                scale = { 0.f };

	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMHeroGrade         Grade = { EKMHeroGrade::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                StatId = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransformSkill = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                IconPdaKey = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BeastRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BeastRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 6;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Beast Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BeastRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BeastRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BeastRow* FKMTable_BeastRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BeastRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BeastRow& FKMTable_BeastRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

