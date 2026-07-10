#pragma once

#include "CoreMinimal.h"
#include "KMGameModeBase.h"
#include "GameplayTagContainer.h"
#include "KMGameModeStage.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeStage : public AKMGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName HeroId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform HeroSpwnTransform;

	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnSpawnHeroInstance(class UKMHeroInstance* newHeroInstance);

public:
	AKMGameModeStage();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

	virtual void OnWorldLoadingComplete_Implementation() override;

	void OnNarrativeEventListening(FGameplayTag newTag);
};
