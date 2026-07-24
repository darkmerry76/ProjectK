#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_Stat_OutLevelUp.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_Stat_OutLevelUpRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_Stat_OutLevelUpRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 캐릭터 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 레벨
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32                Level = { 0 };

	// 기본 체력
	UPROPERTY(EditAnywhere)
	double               Hp = { 0.f };

	// 체력초당회복력
	UPROPERTY(EditAnywhere)
	double               HpRps = { 0.f };

	// 기본 공격력
	UPROPERTY(EditAnywhere)
	double               Atk = { 0.f };

	// 기본 공속
	UPROPERTY(EditAnywhere)
	double               Aspd = { 0.f };

	// 마나
	UPROPERTY(EditAnywhere)
	double               Sp = { 0.f };

	// 마나초당회복력
	UPROPERTY(EditAnywhere)
	double               SpRps = { 0.f };

	// 마나
	UPROPERTY(EditAnywhere)
	double               Mp = { 0.f };

	// 마나초당회복력
	UPROPERTY(EditAnywhere)
	double               MpRps = { 0.f };

	// 민첩성
	UPROPERTY(EditAnywhere)
	double               Dex = { 0.f };

	// 방어력
	UPROPERTY(EditAnywhere)
	double               Def = { 0.f };

	// 치명타
	UPROPERTY(EditAnywhere)
	double               Cri = { 0.f };

	// 치명타확률
	UPROPERTY(EditAnywhere)
	double               CriChange = { 0.f };

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

	// 현재 공단계
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                EmpPhaseCurr = { 0 };

	// 공게이지
	UPROPERTY(EditAnywhere)
	double               EmpCurr = { 0.f };

	// 템포게이지
	UPROPERTY(EditAnywhere)
	double               Tempo = { 0.f };

	// 템포게이초당회복
	UPROPERTY(EditAnywhere)
	double               TempoRps = { 0.f };

	// 스킬해금
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        UnlockSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                SpiritSeal = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        Reward;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_Stat_OutLevelUpRow* FindRowPtr(FName IdKey, int32 LevelKey);
	inline static const FKMTable_Stat_OutLevelUpRow& FindRow(FName IdKey, int32 LevelKey);
	inline static FString MakeTableKeyToString(FName IdKey, int32 LevelKey);

	static const int32   RefTableIndex = 58;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_Stat_OutLevelUp Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_Stat_OutLevelUpRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_Stat_OutLevelUpRow::MakeTableKeyToString(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey, LevelKey);
}

inline const FKMTable_Stat_OutLevelUpRow* FKMTable_Stat_OutLevelUpRow::FindRowPtr(FName IdKey, int32 LevelKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_Stat_OutLevelUpRow>(*MakeTableKeyToString(IdKey, LevelKey));
}

inline const FKMTable_Stat_OutLevelUpRow& FKMTable_Stat_OutLevelUpRow::FindRow(FName IdKey, int32 LevelKey)
{
	return *FindRowPtr(IdKey, LevelKey);
}

