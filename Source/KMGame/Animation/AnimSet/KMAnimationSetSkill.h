#pragma once

#include "CoreMinimal.h"
#include "KMAnimationSetBase.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAnimationSetSkill.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimationSetSkill : public UKMAnimationSetBase
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EKMAnimSetSkillType, TObjectPtr<UAnimMontage>> AnimMontageMap;
};