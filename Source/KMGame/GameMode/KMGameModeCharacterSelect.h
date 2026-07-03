#pragma once

#include "CoreMinimal.h"
#include "KMGameModeBase.h"
#include "KMGameModeCharacterSelect.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMHeroSelectDelegate, const FName& heroTableId);

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeCharacterSelect : public AKMGameModeBase
{
	GENERATED_BODY()
public:
	FKMHeroSelectDelegate HeroSelectDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LatestHeroTableId = TEXT("H_RyuX");

public:
	UFUNCTION(BlueprintCallable)
	void SelectCharacter(class AController* newPlayer, const FName& newCharacterId, bool bForce = false);

protected:
	virtual void RestartPlayer(class AController* NewPlayer) override;
	virtual void BeginPlay() override;

	virtual void OnSpawnCharacterInstance_Implementation(class UKMHeroInstance* newHeroInstance) override;
};
