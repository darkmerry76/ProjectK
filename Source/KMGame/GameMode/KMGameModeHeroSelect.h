#pragma once

#include "CoreMinimal.h"
#include "KMGameModeStage.h"
#include "KMGameModeHeroSelect.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMHeroSelectDelegate, const FName& heroTableId);

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeHeroSelect : public AKMGameModeBase
{
	GENERATED_BODY()
	
public:
	FKMHeroSelectDelegate HeroSelectDelegate;

	UPROPERTY()
	TWeakObjectPtr<class UKMHeroInstance> LatestHeroInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName HeroId = NAME_None;

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

	UFUNCTION(BlueprintNativeEvent)
	void OnSpawnHeroInstance(class UKMHeroInstance* newHeroInstance);
	
	virtual void OnWorldLoadingComplete_Implementation() override;
};