#pragma once

#include "CoreMinimal.h"
#include "DefaultMovementSet/Settings/CommonLegacyMovementSettings.h"
#include "EMMovementSettings.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="MovementSettings")
class EMCOREGAME_API UEMMovementSettings : public UCommonLegacyMovementSettings
{
	GENERATED_BODY()
};