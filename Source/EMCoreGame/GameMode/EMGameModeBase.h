#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EMGameModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API AEMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
};
