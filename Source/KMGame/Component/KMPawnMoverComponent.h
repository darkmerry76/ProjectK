#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "Component/EMPawnMoverComponent.h"
#include "KMPawnMoverComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMSweepPawnMoverHitDelegate, const TArray<FHitResult>& hitResults);

UCLASS(Blueprintable, BlueprintType, DisplayName="PawnMoverComponent", meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMPawnMoverComponent : public UEMPawnMoverComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCurveVector> JumpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpDuration = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpApexHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpHorizontalDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	FVector LatestJumpInputDir = FVector::ZeroVector;

	bool bIsEnableCustomWalking = false;

	UPROPERTY()
	TWeakObjectPtr<class UAnimSequence> CustomWalkingAnimSequence;

public:
	FHitResult BlockHitResult;
	FVector BlockMoveDelta = FVector::ZeroVector;

	FKMSweepPawnMoverHitDelegate SweepPawnHitDelegate;
	FEMCustomMovementDelegate CustomMovementDelegate;
	
public:
	UKMPawnMoverComponent();

	UFUNCTION(BlueprintCallable)
	void JumpCurveWarping();

	UFUNCTION(BlueprintCallable)
	virtual void SetCustomWalkingAnimation(class UAnimSequence* animSequence);

	UFUNCTION(BlueprintCallable)
	virtual void ClearCustomWalkingAnimation();

	UFUNCTION(BlueprintCallable)
	virtual void EnableCustomWalking();

	UFUNCTION(BlueprintCallable)
	virtual void DisableCustomWalking();

	UFUNCTION(BlueprintCallable)
	virtual void StartFollowActor(class AActor* newFollowerActor, FName leaderMontageInstanceId, FName followMontageInstanceId, const FTransform& offsetTransform, float duration = 0.2f);

	UFUNCTION(BlueprintCallable)
	virtual void StopFollowActor(class AActor* followActor, float duration = 0.2f);

	UFUNCTION(BlueprintCallable)
	void RegisterMoveBlockReflection(class AKMCharacter* targetCharacter);

	UFUNCTION(BlueprintCallable)
	void UnregisterMoveBlockReflection(class AKMCharacter* targetCharacter);

	UFUNCTION()
	void OnMoverModeChanged(const FName& previousMode, const FName& newMode);

protected:
	virtual void OnRegister() override;
};