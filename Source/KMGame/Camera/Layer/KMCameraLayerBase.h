#pragma once

#include "CoreMinimal.h"
#include "Sequencer/EMCameraCacheManager.h"
#include "KMCameraLayerBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMCameraLayerBase : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Evalate(float deltaTime, FEMCameraOutput& output); 
};