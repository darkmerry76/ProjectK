#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EMPawnMovementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class EMCOREGAME_API UEMPawnMovementComponent : public UPawnMovementComponent, public IEMCurveWarpingInterface
{
	GENERATED_UCLASS_BODY()

public:
	FEMCustomMovementDelegate CustomMovementDelegate;

public:
	virtual void BindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;
	virtual void UnbindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;

	virtual bool CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations) override;
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
};