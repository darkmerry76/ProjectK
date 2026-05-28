#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KMVoicePDA.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMVoicePDA : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class USoundWave>> SoundWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USoundCue> NarrativeSoundCue;
};