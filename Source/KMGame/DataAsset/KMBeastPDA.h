#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KMBeastPDA.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBeastPDA : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMAnimInstance> AnimInstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMAnimationSetTag> AnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMSoundSetTag> SoundSet;
};