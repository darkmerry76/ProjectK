#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMSoundSetTag.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMSoundSetTag : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<USoundBase>> SoundMap;

public:
	class USoundBase* GetSound(FGameplayTag tag) const;
};