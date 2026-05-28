#pragma once

#include "CoreMinimal.h"
#include "KMStatModifierBase.h"
#include "KMMonsterStatModifier.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMMonsterStatModifier : public UKMStatModifierBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Init();
	virtual void Deinit();

	virtual void ApplyLevel(int32 newLevel, bool bCurrentFull = false) override;
	virtual void ComputeEffectLevelStatInteral(int32 level) override;
};
