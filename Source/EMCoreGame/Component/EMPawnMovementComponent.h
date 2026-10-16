#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EMPawnMovementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class EMCOREGAME_API UEMPawnMovementComponent : public UPawnMovementComponent, public IEMCurveWarpingInterface
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float MaxStepHeight = 45.f;

	UPROPERTY(Category="Pawn Movement: Walking", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint8 bForceNextFloorCheck:1 = true;

	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	uint8 bAlwaysCheckFloor:1 = true;

	UPROPERTY(Category="Pawn Movement (General Settings)", Transient, VisibleInstanceOnly, BlueprintReadWrite)
	uint8 bJustTeleported:1 = true;

	UPROPERTY(Category="Pawn Movement: Walking", VisibleInstanceOnly, BlueprintReadOnly)
	FFindFloorResult CurrentFloor;

	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(ClampMin="0", UIMin="0", ForceUnits=cm))
	float PerchAdditionalHeight = 40.f;

	UPROPERTY(Category="Pawn Movement: Gravity", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector GravityDirection = FVector(0.f, 0.f, -1.f);

	UPROPERTY(Category="Pawn Movement: Gravity", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FQuat WorldToGravityTransform = FQuat::Identity;;

	UPROPERTY(Category="Pawn Movement: Gravity", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FQuat GravityToWorldTransform = FQuat::Identity;;

	UPROPERTY(Category="Pawn Movement: Walking", VisibleAnywhere)
	float WalkableFloorZ = 0.71f;

	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(ClampMin="0", UIMin="0", ForceUnits=cm))
	float PerchRadiusThreshold = 30.f;

	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	uint8 bUseFlatBaseForFloorChecks:1 = false;

	UPROPERTY(Category="Pawn Movement: Walking", EditAnywhere, BlueprintReadWrite)
	uint8 bMaintainHorizontalGroundVelocity:1 = false;

	EMovementMode MovementMode = MOVE_Falling;

	EEMCustomMovementMode CustomMovementMode = EEMCustomMovementMode::CMODE_Walking;

	bool bHasCustomGravity;

public:
	FEMCustomMovementDelegate CustomMovementDelegate;

public:
	virtual void BindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;
	virtual void UnbindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;

	virtual bool CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations) override;
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

	virtual bool IsAir() const;

	bool FloorSweepTest(FHitResult& outHit, const FVector& start, const FVector& end,
		ECollisionChannel traceChannel,
		const struct FCollisionShape& collisionShape,
		const struct FCollisionQueryParams& params,
		const struct FCollisionResponseParams& responseParam) const;

	bool ComputePerchResult(const float testRadius, const FHitResult& hitResult, const float maxFloorDist, FFindFloorResult& outPerchFloorResult) const;
	
	void ComputeFloorDist(const FVector& pawnLocation, float lineDistance, float sweepDistance, FFindFloorResult& outFloorResult, float sweepRadius, const FHitResult* downwardSweepResult = nullptr) const;
	void FindFloor(const FVector& pawnLocation, FFindFloorResult& outFloorResult, bool bCanUseCachedLocation, const FHitResult* downwardSweepResult = nullptr) const;

	UFUNCTION(BlueprintPure, Category="Pawn|Components|CharacterMovement")
	FVector GetGravityDirection() const { return GravityDirection; }

	UFUNCTION(BlueprintPure, Category="Pawn|Components|CharacterMovement")
	bool HasCustomGravity() const { return bHasCustomGravity; }
	
	FVector::FReal GetGravitySpaceZ(const FVector& Vector) const { return Vector.Dot(-GetGravityDirection()); };

	FVector ProjectToGravityFloor(const FVector& Vector) const { return FVector::VectorPlaneProject(Vector, GetGravityDirection()); }

	FQuat GetWorldToGravityTransform() const { return WorldToGravityTransform; };

	FVector RotateWorldToGravity(const FVector& Gravity) const { return GravityToWorldTransform.RotateVector(Gravity); }
	
	FVector RotateGravityToWorld(const FVector& World) const { return WorldToGravityTransform.RotateVector(World); }

	virtual bool IsWithinEdgeTolerance(const FVector& location, const FVector& testImpactPoint, const float radius) const;

	bool ShouldComputePerchResult(const FHitResult& hitResult, bool bCheckRadius) const;

	float GetPerchRadiusThreshold() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
	virtual bool IsWalkable(const FHitResult& hitResult) const;

	bool CanStepUp(const FHitResult& hitResult) const;
	bool StepUp(const FVector& gravDir, const FVector& delta, const FHitResult &hitResult, FStepDownResult* outStepDownResult = nullptr);

	void SetMovementMode(EMovementMode newMovementMode, uint8 newCustomMode = 0);

protected:
	virtual bool CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations);
	virtual bool CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations);
	virtual bool CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations);

	virtual void ProcessLanded(const FHitResult& hitResult, float remainingTime, int32 iterations);
	virtual void ProcessWallHit(const FHitResult hitResult);
	virtual void ProcessCeilingHit(const FHitResult hitResult);
	
	virtual bool HasValidData() const;

	virtual float GetScaledRadius() const;
	virtual float GetScaledHalfHeight() const;
	virtual class UPrimitiveComponent* GetMovementBase() const;
	
	bool IsDynamicBase() const;

	float GetValidPerchRadius() const;
	bool ShouldCheckForValidLandingSpot(float deltaTime, const FVector& delta, const FHitResult& hit) const;

	static const float MIN_TICK_TIME;
	static const float MIN_FLOOR_DIST;
	static const float MAX_FLOOR_DIST;
	static const float SWEEP_EDGE_REJECT_DISTANCE;
	static const float MAX_STEP_SIDE_Z;
};