#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Character.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_CharacterRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_CharacterRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterType     Type = { EKMCharacterType::Hero };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEMPrimaryAssetRef   pdaKey = { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMCharacterLocationType LocationType = { EKMCharacterLocationType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                scale = { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                StatId = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_CharacterRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_CharacterRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 9;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Character Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_CharacterRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_CharacterRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_CharacterRow* FKMTable_CharacterRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_CharacterRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_CharacterRow& FKMTable_CharacterRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

