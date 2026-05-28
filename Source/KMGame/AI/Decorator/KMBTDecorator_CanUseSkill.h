#pragma once

#include "CoreMinimal.h"
#include "KMBTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "KMBTDecorator_CanUseSkill.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTDecorator_CanUseSkill : public UKMBTDecorator
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTDecorator_HasEnemy : public UBTDecorator_BlackboardBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsInverse = false;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
};