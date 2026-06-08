#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMAnimationSetTag.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMAnimationSetTag : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimMontageMap;
};