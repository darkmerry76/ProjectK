#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "KMGameSettings.generated.h"

UCLASS(config=Game, defaultconfig, Blueprintable, BlueprintType)
class KMGAME_API UKMGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere, Category = "Map")
	FString MapAreaPath;
};