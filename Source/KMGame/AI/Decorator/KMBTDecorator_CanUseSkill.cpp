#include "KMBTDecorator_CanUseSkill.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMMonsterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMGameObjectSubsystem.h"

UKMBTDecorator_CanUseSkill::UKMBTDecorator_CanUseSkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanUseSkill");
}

bool UKMBTDecorator_CanUseSkill::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
{
	AAIController* aiController = ownerComp.GetAIOwner();
	check(IsValid(aiController));
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character));
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));

	UBlackboardComponent* blackboardComponent = aiController->GetBlackboardComponent();
	check(IsValid(blackboardComponent));

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

bool UKMBTDecorator_HasEnemy::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
{
	AAIController* aiController = ownerComp.GetAIOwner();
	check(IsValid(aiController));
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character));
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));

	UBlackboardComponent* blackboardComponent = aiController->GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	bool bResult = IsValid(targetCharacter) == !bIsInverse;
	if (bResult)
	{
		return  true;
	}
	return false;
}

void UKMBTDecorator_HasEnemy::OnBecomeRelevant(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	Super::OnBecomeRelevant(ownerComp, nodeMemory);
}

EBlackboardNotificationResult UKMBTDecorator_HasEnemy::OnBlackboardKeyValueChange(const UBlackboardComponent& blackboard, FBlackboard::FKey changedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(blackboard.GetBrainComponent());
	if (!IsValid(BehaviorComp))
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (BlackboardKey.GetSelectedKeyID() == changedKeyID)
	{
		ConditionalFlowAbort(*BehaviorComp, EBTDecoratorAbortRequest::ConditionPassing);
	}

	return EBlackboardNotificationResult::ContinueObserving;
}