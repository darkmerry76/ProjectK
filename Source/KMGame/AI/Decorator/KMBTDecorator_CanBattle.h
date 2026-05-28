#pragma once

#include "CoreMinimal.h"
#include "KMBTDecorator.h"
#include "KMBTDecorator_CanBattle.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTDecorator_CanBattle : public UKMBTDecorator
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};