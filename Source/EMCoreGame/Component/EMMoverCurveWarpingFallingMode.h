#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "MovementMode.h"
#include "DefaultMovementSet/Modes/FallingMode.h"
#include "EMMoverCurveWarpingFallingMode.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="CurveWarpingFallingMode", meta=(BlueprintSpawnableComponent))
class EMCOREGAME_API UEMMoverCurveWarpingFallingMode : public UFallingMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Mover, EditAnywhere, BlueprintReadWrite)
	bool bRespectDistanceOverWalkableSurfaces = false;

	FEMGetMoveDeltaFalling GetMoveDeltaFallingDelegate;
	
public:
	UEMMoverCurveWarpingFallingMode();
	
	virtual void OnRegistered(const FName newModeName, const FMoverSimContext& simContext) override;
	virtual void OnUnregistered(const FMoverSimContext& simContext) override;

	UFUNCTION(BlueprintCallable)
	virtual void BindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent);

	UFUNCTION(BlueprintCallable)
	virtual void UnbindCurveWarpingComponent(class UEMCurveWarpingComponent* curveWarpingComponent);

	virtual void SimulationTick_Implementation(const FSimulationTickParams& params, FMoverTickEndData& outputState) override;
	virtual void GenerateMove_Implementation(const FMoverSimContext& simContext, const FMoverTickStartData& startState, const FMoverTimeStep& timeStep, FProposedMove& outProposedMove) const override;

protected:
	virtual void Activate(const FMoverEventContext& context, FName prevModeName,
		const FMoverSimContext& simContext, const FMoverTickStartData& startState, FMoverSyncState* outSyncState, FMoverAuxStateContext* outAuxState) override;

	virtual void ProcessLanded(const FFloorCheckResult& floorResult, FVector location, FVector& velocity, FRelativeBaseInfo& baseInfo, FMoverTickEndData& tickEndData) const override;

public:
	UPROPERTY()
	TObjectPtr<const class UCommonLegacyMovementSettings> CommonLegacySettings;
	
	inline static const FName ModeName = TEXT("CurveWarpingFalling");
};