#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMAbility.h"
#include "KMAbilitySkillDirectionTag.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySkillDirectionTag : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DefaultAnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Front.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultDirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Angle0_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Front.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle0_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleL45_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL45_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleL90_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL90_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleL135_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleL135_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Angle180_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle180_DirectionWeight = -1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleR45_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR45_DirectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleR90_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR90_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AngleR135_AnimTag = FGameplayTag::RequestGameplayTag(TEXT("Anim.Dash.Back.0"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleR135_DirectionWeight = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveBase> Curve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MotionWarpingTarget = TEXT("WarpingTarget");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetDistance = 200.f;

public:
	virtual void Activate() override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintPure)
	FGameplayTag Get8WayDirectionAnimTag(EKM8WayDirection direction) const;

	UFUNCTION(BlueprintPure)
	float Get8WayDirectionWeight(EKM8WayDirection direction) const;

	UFUNCTION(BlueprintCallable)
	void ApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration, bool bIngnoreZ = true);

	UFUNCTION(BlueprintImplementableEvent)
	void OnApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration);
};