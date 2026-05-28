#pragma once

#include "CoreMinimal.h"
#include "KMAnimationSetBase.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAnimationSetEffect.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimationSetEffect : public UKMAnimationSetBase
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EKMAnimSetEffectType, TObjectPtr<UAnimMontage>> AnimMontageMap;
};