#pragma once

#include "CoreMinimal.h"
#include "KMStatModifierBase.h"
#include "KMHeroStatModifier.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMHeroStatModifier : public UKMStatModifierBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Init() override;
	virtual void Deinit() override;

protected:
	virtual void ComputeEffectLevelStatInteral(int32 level);
};
