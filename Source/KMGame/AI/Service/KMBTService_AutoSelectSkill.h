#pragma once
#include "KMBTService.h"
#include "KMBTService_AutoSelectSkill.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTService_AutoSelectSkill : public UKMBTService
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void OnSearchStartAI(class AAIController* OwnerController, class APawn* ControlledPawn) override;
	virtual void TickNode(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	struct FKMSkillKey EvaluateBestSkill(const class UKMCharacterInstance* casterCharacterInstance, const class UKMCharacterInstance* targetCharacterInstance) const;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTService_PerceptionTarget : public UKMBTService
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void OnSearchStartAI(class AAIController* OwnerController, class APawn* ControlledPawn) override;
	virtual void TickNode(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	const struct FKMSkillKey* EvaluateBestSkill(const class UKMSkillHandler* skillHandler) const;
};