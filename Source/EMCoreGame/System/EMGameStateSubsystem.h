#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "EMGameStateSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMGameStateSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};
