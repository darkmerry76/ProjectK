#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "EMDataTable.h"
#include "KMTableEnums.h"
#include "KMTableStructures.h"
#include "KMTable_SkillCondition.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTable_SkillConditionRow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FKMTable_SkillConditionRow : public FEM_TableBaseRow
{
	GENERATED_USTRUCT_BODY()

	// 아이디
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName                Id = { NAME_None };

	// 스킬 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString              Name;

	// 스킬 효과 타이밍 (idx)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        EffectTiming;

	// 스킬 효과 분류
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        Effect;

	// 맞는 부위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                HitPoint = { 0 };

	// 태그조건
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName>        ReadTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 Grabable = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 Carryable = { false };

	// 페어여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 PairEff = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMTargetLockonType  LockonType = { EKMTargetLockonType::None };

	// 이전스킬
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool                 IsCancel = { false };

	// 이전스킬
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName                TransitionSkill = { NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMSkillTransitionBehaviorType TransitionSkillBehavior = { EKMSkillTransitionBehaviorType::None };

	// 스킬발동 로코모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKMLocomotionStateType LocomotionState = { EKMLocomotionStateType::None };

	// 스킬발동 최대거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                TargetRange = { 0.f };

	// 스킬발동 최소거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                TargetRangeMin = { 0.f };

	// 스킬발동각
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                TargetDir = { 0.f };

	// 입력발동각
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                InputDir = { 0.f };

	// 적이 공중일때 발동각
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                AirAngle = { 0.f };

	// 기습대상방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                StealthDir = { 0 };

	// 무게 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float                WeightRatio = { 0.f };

	// 스킬발동캐릭터타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>        TargetType;

	// 오브젝트 반격 타입(대분류)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MajorReactType = { 0 };

	// (오브젝트 반격 타입)소분류
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32                MinorReactType = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline virtual void PostLoadRowTable() override;

	inline static const FKMTable_SkillConditionRow* FindRowPtr(FName IdKey);
	inline static const FKMTable_SkillConditionRow& FindRow(FName IdKey);
	inline static FString MakeTableKeyToString(FName IdKey);

	static const int32   RefTableIndex = 48;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KMTable_SkillCondition Inline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FKMTable_SkillConditionRow::PostLoadRowTable()
{
	BaseScriptStruct = this->StaticStruct();
}

inline FString FKMTable_SkillConditionRow::MakeTableKeyToString(FName IdKey)
{
	return FEMDataTableHelper::Get().MakeTableIndexToString(IdKey);
}

inline const FKMTable_SkillConditionRow* FKMTable_SkillConditionRow::FindRowPtr(FName IdKey)
{
	return FEMDataTableHelper::Get().FindRowPtr<FKMTable_SkillConditionRow>(*MakeTableKeyToString(IdKey));
}

inline const FKMTable_SkillConditionRow& FKMTable_SkillConditionRow::FindRow(FName IdKey)
{
	return *FindRowPtr(IdKey);
}

