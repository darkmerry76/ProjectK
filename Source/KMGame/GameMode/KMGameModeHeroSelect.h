#pragma once

#include "CoreMinimal.h"
#include "KMGameModeStage.h"
#include "KMGameModeHeroSelect.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMHeroSelectDelegate, const FName& heroTableId);

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeHeroSelect : public AKMGameModeStage
{
	GENERATED_BODY()
	
public:
	FKMHeroSelectDelegate HeroSelectDelegate;

	UPROPERTY()
	TWeakObjectPtr<class UKMHeroInstance> LatestHeroInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultSelectTableId;

public:
	UFUNCTION(BlueprintCallable)
	void SelectHero(class AController* newPlayer, const FName& newHeroId, bool bForce = false);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();

protected:
	virtual void RestartPlayer(class AController* NewPlayer) override;
	virtual void BeginPlay() override;

	virtual void OnSpawnHeroInstance_Implementation(class UKMHeroInstance* newHeroInstance) override;
	virtual void OnWorldLoadingComplete_Implementation() override;
};