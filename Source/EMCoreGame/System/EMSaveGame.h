#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EMSaveGame.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMSaveGame : public USaveGame
{
	GENERATED_BODY()
};
