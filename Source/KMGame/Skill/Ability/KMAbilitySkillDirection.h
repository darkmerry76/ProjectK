#pragma once

#include "CoreMinimal.h"
#include "KMAbility.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAbilitySkillDirection.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySkillDirection : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType DefaultCancelAnimType = EKMAnimSetSkillType::Front_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultDirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType Angle0_CancelAnimType = EKMAnimSetSkillType::Front_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle0_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleL45_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL45_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleL90_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL90_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleL135_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL135_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType Angle180_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle180_DirectionWeight = -1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleR45_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR45_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleR90_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR90_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAnimSetSkillType AngleR135_CancelAnimType = EKMAnimSetSkillType::Back_Dash_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR135_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveBase> Curve;
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintPure)
	EKMAnimSetSkillType Get8WayDirectionAnimSet(EKM8WayDirection direction) const;

	UFUNCTION(BlueprintPure)
	float Get8WayDirectionWeight(EKM8WayDirection direction) const;

	UFUNCTION(BlueprintCallable)
	void ApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration, bool bIngnoreZ = true);

	UFUNCTION(BlueprintImplementableEvent)
	void OnApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration);
};