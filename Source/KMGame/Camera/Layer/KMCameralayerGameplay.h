#pragma once

#include "CoreMinimal.h"
#include "KMCameralayerBase.h"
#include "KMCameralayerGameplay.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract)
class KMGAME_API UKMCameralayerGameplay : public UKMCameralayerBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void Evaluate(float deltaTime, FEMCameraOutput& output) override;
};