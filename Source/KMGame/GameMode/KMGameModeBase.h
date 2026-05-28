#pragma once

#include "CoreMinimal.h"
#include "GameMode/EMGameModeBase.h"
#include "KMGameModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeBase : public AEMGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", AllowedClasses="World"))
	FSoftObjectPath InitMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName HeroId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform HeroSpwnTransform;

	virtual void RestartPlayer(AController* NewPlayer) override;
	
public:
	virtual void BeginPlay() override;
	bool IsInitMap(const UWorld* otherWorld) const;
};
