#pragma once

#include "CoreMinimal.h"
#include "KMBTDecorator.h"
#include "KMBTDecorator_CanBattle.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTDecorator_CanBattle : public UKMBTDecorator
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const override;
};