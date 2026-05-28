#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "EMPlayerSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMPlayerSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};