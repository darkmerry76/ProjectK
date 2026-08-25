#pragma once

#include "CoreMinimal.h"
#include "KMStatModifierBase.h"
#include "KMInteractiveStatModifier.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMInteractiveStatModifier : public UKMStatModifierBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Init();
	virtual void Deinit();
	
	virtual void ComputeEffectLevelStatInteral(int32 level) override;
};
