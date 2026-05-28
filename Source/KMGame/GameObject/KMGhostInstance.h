#pragma once

#include "CoreMinimal.h"
#include "KMCharacterInstance.h"
#include "KMGhostInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMGhostInstance : public UKMCharacterInstance
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool CanLockOn() const override { return false; };
	virtual bool CanBeTargeted() const override { return false; };
};