#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KMItemPDA.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMItemPDA : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AKMItemAppearanceActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMItemAppearanceInstance> InstanceClass;
};