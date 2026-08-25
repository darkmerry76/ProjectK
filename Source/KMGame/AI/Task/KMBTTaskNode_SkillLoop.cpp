#include "KMBTTaskNode_SkillLoop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_SkillLoop::UKMBTTaskNode_SkillLoop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "SkillLoop";
	bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_SkillLoop::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_SkillLoop::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));
	
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));

	if (skillHandler->IsSkillAvailable(skillKey))
	{
		FinishLatentTask(ownerComp, EBTNodeResult::Succeeded);
	}
}