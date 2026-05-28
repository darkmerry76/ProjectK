#pragma once

#include "CoreMinimal.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAbilitySet.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySetBase : public UDataAsset
{
	GENERATED_UCLASS_BODY()
};

UCLASS(Blueprintable, BlueprintType)
class UKMAbilityEffectSet : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EKMAnimSetEffectType, TSubclassOf<class UKMAbility>> AbilityMap;
};