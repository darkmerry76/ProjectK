#pragma once

#include "CoreMinimal.h"
#include "Component/EMPawnMovementComponent.h"
#include "KMPawnMovementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMPawnMovementComponent : public UEMPawnMovementComponent
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations) override;

protected:
	virtual bool CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations) override;
	virtual bool CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations) override;
	virtual bool CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations) override;
	virtual void ProcessLanded(const FHitResult& hitResult, float remainingTime, int32 iterations) override;

	float GetSimulationTimeStep(float RemainingTime, int32 Iterations) const;

	bool IsValidLandingSpot(const FVector& location, const FHitResult& hitResult) const;

	virtual float GetScaledRadius() const override;
	virtual float GetScaledHalfHeight() const override;

	virtual class UPrimitiveComponent* GetMovementBase() const override;
	
protected:
	int32 MaxSimulationIterations = 1;
	int32 MaxSimulationTimeStep = 1;
	FHitResult BlockHitResult;
};