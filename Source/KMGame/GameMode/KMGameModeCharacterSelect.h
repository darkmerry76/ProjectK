#pragma once

#include "CoreMinimal.h"
#include "KMGameModeBase.h"
#include "KMGameModeCharacterSelect.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeCharacterSelect : public AKMGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectCharacter(class AController* newPlayer, const FName& newCharacterId);

protected:
	virtual void RestartPlayer(class AController* NewPlayer) override;
	virtual void BeginPlay() override;

	virtual void OnSpawnCharacterInstance_Implementation(class UKMHeroInstance* newHeroInstance) override;
};
