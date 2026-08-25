#pragma once
#include "KMBTService.h"
#include "KMBTService_AutoSelectSkill.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTService_AutoSelectSkill : public UKMBTService
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void OnSearchStartAI(class AAIController* ownerController, class APawn* controlledPawn) override;
	virtual void TickNode(class UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds) override;

protected:
	struct FKMSkillKey EvaluateBestSkill(const class UKMCharacterInstance* casterCharacterInstance, const class UKMCharacterInstance* targetCharacterInstance) const;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTService_PerceptionTarget : public UKMBTService
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void OnSearchStartAI(class AAIController* ownerController, class APawn* controlledPawn) override;
	virtual void TickNode(class UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds) override;

protected:
	const struct FKMSkillKey* EvaluateBestSkill(const class UKMSkillHandler* skillHandler) const;
};