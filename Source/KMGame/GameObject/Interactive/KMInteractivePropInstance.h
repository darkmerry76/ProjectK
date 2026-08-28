#pragma once

#include "CoreMinimal.h"
#include "KMInteractiveInstance.h"
#include "KMInteractivePropInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMInteractivePropInstance : public UKMInteractiveInstance
{
	GENERATED_UCLASS_BODY()

protected:
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMBreakableInstance : public UKMInteractivePropInstance
{
	GENERATED_UCLASS_BODY()

protected:
};