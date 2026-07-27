#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "Component/EMCharacterMovementComponent.h"
#include "KMCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EKMCustomMovementMode : uint8
{
	None,
	CMODE_Walking,
	CMODE_Flying,
	CMODE_Jump,
	CMODE_Falling,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKMCustomMovementDelegate, float, deltaTime, int32, iterations);
DECLARE_MULTICAST_DELEGATE_OneParam(FKMSweepPawnHitDelegate, const TArray<FHitResult>& hitResults);

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMCharacterMovementComponent : public UEMCharacterMovementComponent, public IEMCurveWarpingInterface
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FKMCustomMovementDelegate CustomMovementDelegate;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UClass* ActorClassFilter;

	UPROPERTY()
	TObjectPtr<UAnimMontage> activeJumpAnimMontage = nullptr;

	float FallTime = 0.f;

	EKMCustomMovementMode MovementModeEx;

	FVector LatestJumpInputDir = FVector::ZeroVector;

	FKMSweepPawnHitDelegate SweepPawnHitDelegate; 

public:
	void SetCustomMovementMode(EKMCustomMovementMode newCustomMovementMode);
	bool IsCustomMovementMode(EKMCustomMovementMode customMovementMode) const;

	void CustomJump();

	virtual bool CustomMovement(const FVector& adjusted, float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlayCurveWarping(UCurveBase* newCurveAsset, FVector newTargetLocation, float newPlayLength, float newZScale, bool bIgnoreZ, bool bAutoEndingWalk);

	void StartCurveEndingFalling(const class UCurveVector* curveVector);

	bool IsOnGround() const;
	bool IsAir() const;

	bool IsCustomRun() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *thisTickFunction) override;
	virtual void PhysCustom(float deltaTime, int32 iterations) override;

	virtual void OnMovementUpdated(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) override;

	bool CustomMovementFalling(const FVector& adjusted, float deltaTime);
	bool CustomMovementFlying(const FVector& adjusted, float deltaTime);
	
	virtual void StartFalling(int32 iterations, float remainingTime, float timeTick, const FVector& delta, const FVector& subLoc) override;
	
	UFUNCTION()
	void OnJumpInterrupt(const FVector& moveDelta, EEMCurveWarpingInteruptType type);

	void StartCustomFalling(const FVector& latestMoveDelta);
	void UpdateCustomFalling(float deltaTime);

	class AKMCharacter* GetOwnerCharacter() const;
	class UKMCharacterInstance* GetOwnerCharacterInstance() const;

	void ProcessOverlapDamage(float deltaSeconds, const FVector& oldLocation, const FVector& newLocation);
	
};