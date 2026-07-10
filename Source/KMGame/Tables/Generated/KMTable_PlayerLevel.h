#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_PlayerLevel.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_PlayerLevelRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_PlayerLevelRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 플레이어 고유 레벨 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32                Id = { 0 };

	// 레벨업하는데 초과 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MaxExp = { 0 };

	// 체력보정률
	UPROPERTY(EditAnywhere)
	double               Hp = { 0.f };

	// 체력초당회복력
	UPROPERTY(EditAnywhere)
	double               HpRps = { 0.f };

	// 공격력보정률
	UPROPERTY(EditAnywhere)
	double               Atk = { 0.f };

	// 공속보정률
	UPROPERTY(EditAnywhere)
	double               Aspd = { 0.f };

	// 마나보정률
	UPROPERTY(EditAnywhere)
	double               Sp = { 0.f };

	// 마나초당회복률
	UPROPERTY(EditAnywhere)
	double               SpRps = { 0.f };

	// 마나보정률
	UPROPERTY(EditAnywhere)
	double               Mp = { 0.f };

	// 마나초당회복률
	UPROPERTY(EditAnywhere)
	double               MpRps = { 0.f };

	// 민첩성보정률
	UPROPERTY(EditAnywhere)
	double               Dex = { 0.f };

	// 방어력보정률
	UPROPERTY(EditAnywhere)
	double               Def = { 0.f };

	// 치명타보정률
	UPROPERTY(EditAnywhere)
	double               Cri = { 0.f };

	// 치명타확률보정률
	UPROPERTY(EditAnywhere)
	double               CriChange = { 0.f };

	// 기본이동 속도보정률
	UPROPERTY(EditAnywhere)
	double               Mov = { 0.f };

	// 달리기 속도보정률
	UPROPERTY(EditAnywhere)
	double               Run = { 0.f };

	// 공게이지
	UPROPERTY(EditAnywhere)
	double               Emp = { 0.f };

	// 공단계
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                EmpPhase = { 0 };

	// 공게이지
	UPROPERTY(EditAnywhere)
	double               EmpCurr = { 0.f };

	// 템포게이지
	UPROPERTY(EditAnywhere)
	double               Tempo = { 0.f };

	// 템포게이초당회복
	UPROPERTY(EditAnywhere)
	double               TempoRps = { 0.f };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_PlayerLevelRow* FindRowPtr(int32 IdKey);
	inline static const FKMTable_PlayerLevelRow& FindRow(int32 IdKey);
	inline static FString MakeTableKeyToString(int32 IdKey);

	static const int32   RefTableIndex = 34;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_PlayerLevel Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_PlayerLevelRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_PlayerLevelRow::MakeTableKeyToString(int32 IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_PlayerLevelRow* FKMTable_PlayerLevelRow::FindRowPtr(int32 IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_PlayerLevelRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_PlayerLevelRow& FKMTable_PlayerLevelRow::FindRow(int32 IdKey)
{
	return *FindRowPtr(IdKey);
}

