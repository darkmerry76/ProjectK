#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_UnlockContentMainLobby.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_UnlockContentMainLobbyRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_UnlockContentMainLobbyRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 언락 콘텐츠 Id
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 IsShowLeft = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                UnlockContentId = { NAME_None };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_UnlockContentMainLobbyRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_UnlockContentMainLobbyRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 65;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_UnlockContentMainLobby Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_UnlockContentMainLobbyRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_UnlockContentMainLobbyRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_UnlockContentMainLobbyRow* FKMTable_UnlockContentMainLobbyRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_UnlockContentMainLobbyRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_UnlockContentMainLobbyRow& FKMTable_UnlockContentMainLobbyRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

