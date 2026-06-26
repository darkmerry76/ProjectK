#pragma once

#include "CoreMinimal.h"
#include "KMCameralayerBase.h"
#include "KMCameralayerOverlaySequence.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract)
class KMGAME_API UKMCameralayerOverlaySequence : public UKMCameralayerBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	FEMCameraOutput RelativeCameraData;
	
public:
	void SetRelativeCameraData(const FEMCameraOutput& newRelativeCameraData);
	
protected:
	virtual void Evaluate(float deltaTime, FEMCameraOutput& output) override;
};