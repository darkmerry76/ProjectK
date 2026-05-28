#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KMCharacterPDA.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMCharacterPDA : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AKMCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMCharacterInstance> InstanceClass;
};