#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_BaseStat.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_BaseStatRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_BaseStatRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 기본 체력
	UPROPERTY(EditAnywhere)
	double               Hp = { 0.f };

	// 체력초당회복
	UPROPERTY(EditAnywhere)
	double               HpRps = { 0.f };

	// 체력초당회복
	UPROPERTY(EditAnywhere)
	double               HpCurr = { 0.f };

	// 기본 공격력
	UPROPERTY(EditAnywhere)
	double               Atk = { 0.f };

	// 기본 공속
	UPROPERTY(EditAnywhere)
	double               AtkSpd = { 0.f };

	// 마나
	UPROPERTY(EditAnywhere)
	double               Sp = { 0.f };

	// 마차초당회복
	UPROPERTY(EditAnywhere)
	double               SpRps = { 0.f };

	// 시작마나비율
	UPROPERTY(EditAnywhere)
	double               SpCurr = { 0.f };

	// 마나
	UPROPERTY(EditAnywhere)
	double               Mp = { 0.f };

	// 마차초당회복
	UPROPERTY(EditAnywhere)
	double               MpRps = { 0.f };

	// 시작마나비율
	UPROPERTY(EditAnywhere)
	double               MpCurr = { 0.f };

	// 민첩성
	UPROPERTY(EditAnywhere)
	double               Dex = { 0.f };

	// 방어력
	UPROPERTY(EditAnywhere)
	double               Def = { 0.f };

	// 치명타
	UPROPERTY(EditAnywhere)
	double               Cri = { 0.f };

	// 치명타 확률
	UPROPERTY(EditAnywhere)
	double               CriChance = { 0.f };

	// 달리기 속도
	UPROPERTY(EditAnywhere)
	double               Mov = { 0.f };

	// 달리기속도
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

	// 현재 공단계
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                EmpPhaseCurr = { 0 };

	// 템포게이지
	UPROPERTY(EditAnywhere)
	double               Tempo = { 0.f };

	// 템포게이초당회복
	UPROPERTY(EditAnywhere)
	double               TempoRps = { 0.f };

	// 템포게이초당회복
	UPROPERTY(EditAnywhere)
	double               TempoCurr = { 0.f };

	// 레벨별 보정량 (StatPerLevel 테이블 참조)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                StatPerLevelId = { NAME_None };

	// 선공여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 InitiativeAttack = { false };

	// 적 인지 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                PerceptionDistance = { 0 };

	// 적 인지 각도 (0 ~ 360) 0 == 360
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                PerceptionAngle = { 0 };

	// 최대 추격 범위 (cm단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MaxChasingDistance = { 0 };

	// 최대 전투 범위 (cm단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MaxCombatRange = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_BaseStatRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_BaseStatRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 2;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_BaseStat Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_BaseStatRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_BaseStatRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_BaseStatRow* FKMTable_BaseStatRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_BaseStatRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_BaseStatRow& FKMTable_BaseStatRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

