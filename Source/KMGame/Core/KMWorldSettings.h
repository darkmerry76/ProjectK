#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "KMWorldSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API AKMWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsShowLoadingScreen = false;
};
