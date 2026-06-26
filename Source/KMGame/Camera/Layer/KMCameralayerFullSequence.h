#pragma once

#include "CoreMinimal.h"
#include "KMCameralayerBase.h"
#include "KMCameralayerFullSequence.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract)
class KMGAME_API UKMCameralayerFullSequence : public UKMCameralayerBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void Evaluate(float deltaTime, FEMCameraOutput& output) override;
};