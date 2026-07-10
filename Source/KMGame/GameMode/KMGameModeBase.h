#pragma once

#include "CoreMinimal.h"
#include "GameMode/EMGameModeBase.h"
#include "KMGameModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMGameModeBase : public AEMGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", AllowedClasses="World"))
	FSoftObjectPath InitMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", AllowedClasses="World"))
	bool bIsAutoAttachStateWidget = true;
	
	UPROPERTY()
	TObjectPtr<class UKMUserWidget> StateWidget;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	
	bool IsInitMap(const UWorld* otherWorld) const;

	UFUNCTION(BlueprintNativeEvent)
	void OnWorldLoadingComplete();

	void SpawnStateWidget();
};
