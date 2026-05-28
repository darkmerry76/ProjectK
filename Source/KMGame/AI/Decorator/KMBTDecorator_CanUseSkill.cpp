#include "KMBTDecorator_CanUseSkill.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "GameObject/KMMonsterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMTargetSubsystem.h"

UKMBTDecorator_CanUseSkill::UKMBTDecorator_CanUseSkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanUseSkill");
}

bool UKMBTDecorator_CanUseSkill::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* aiController = OwnerComp.GetAIOwner();
	check(IsValid(aiController) == true);
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character) == true);
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance) == true);

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler) == true);

	UBlackboardComponent* blackboardComponent = aiController->GetBlackboardComponent();
	check(IsValid(blackboardComponent) == true);

	AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	if (!IsValid(targetCharacter))
	{
		return false;
	}

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));

/*	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem) == true);

	TSharedPtr<FKMLockOnCluster> lockOnCluster = targetSubsystem->GetLockOnCluster(ownerCharacterInstance->GetId());
	if (lockOnCluster.IsValid() == false)
	{
		return false;
	}*/
	return skillHandler->CanUseSkill(skillKey, nullptr);
}

UKMBTDecorator_HasEnemy::UKMBTDecorator_HasEnemy(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("HasEnemy");

	FlowAbortMode = EBTFlowAbortMode::Self;
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UKMBTDecorator_HasEnemy, BlackboardKey), AActor::StaticClass());

	bNotifyTick = true;
}

bool UKMBTDecorator_HasEnemy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* aiController = OwnerComp.GetAIOwner();
	check(IsValid(aiController) == true);
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character) == true);
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance) == true);

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler) == true);

	UBlackboardComponent* blackboardComponent = aiController->GetBlackboardComponent();
	check(IsValid(blackboardComponent) == true);

	AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	bool bResult = IsValid(targetCharacter) == !bIsInverse;
	if (bResult)
	{
		return  true;
	}
	return false;
}

void UKMBTDecorator_HasEnemy::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

EBlackboardNotificationResult UKMBTDecorator_HasEnemy::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*)Blackboard.GetBrainComponent();
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (BlackboardKey.GetSelectedKeyID() == ChangedKeyID)
	{
		ConditionalFlowAbort(*BehaviorComp, EBTDecoratorAbortRequest::ConditionPassing);
	}

	return EBlackboardNotificationResult::ContinueObserving;
}