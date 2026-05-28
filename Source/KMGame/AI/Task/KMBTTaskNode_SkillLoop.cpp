#include "KMBTTaskNode_SkillLoop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/KMSkillTypes.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_SkillLoop::UKMBTTaskNode_SkillLoop(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "SkillLoop";
	bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_SkillLoop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_SkillLoop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance) == true);

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler) == true);
	
	UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent) == true);

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));

	if (skillHandler->IsSkillAvailable(skillKey) == true)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}