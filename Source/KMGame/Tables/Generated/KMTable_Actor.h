#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Actor.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_ActorRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_ActorRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 액터 id
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	// 액터설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              pdaKey;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_ActorRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_ActorRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Actor Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_ActorRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_ActorRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_ActorRow* FKMTable_ActorRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_ActorRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_ActorRow& FKMTable_ActorRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

