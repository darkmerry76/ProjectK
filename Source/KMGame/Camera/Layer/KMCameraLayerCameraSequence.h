#pragma once

#include "CoreMinimal.h"
#include "KMCameraLayerBase.h"
#include "KMCameraLayerCameraSequence.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMCameraLayerCameraSequence : public UKMCameraLayerBase
{
	GENERATED_UCLASS_BODY()
public:

protected:
	virtual void Evalate(float deltaTime, FEMCameraOutput& output) override;
};