#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_GameEvent.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_GameEventRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_GameEventRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 게임이벤트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 이벤트 시작 날짜와 시간 (UTC+0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              EventStartTime;

	// 이벤트 시작 종료와 시간 (UTC+0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              EventEndTime;

	// 시작, 끝 날짜가 없는 반복형 이벤트의 쿨타임 (h기준으로 작성)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              EventCoolTime;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_GameEventRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_GameEventRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 18;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_GameEvent Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_GameEventRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_GameEventRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_GameEventRow* FKMTable_GameEventRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_GameEventRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_GameEventRow& FKMTable_GameEventRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

