#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "EMNetworkSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMNetworkSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};
