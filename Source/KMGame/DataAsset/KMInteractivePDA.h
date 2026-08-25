#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KMInteractivePDA.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMInteractivePDA : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AKMInteractiveActorBase> InteractiveClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMInteractiveInstance> InstanceClass;
};