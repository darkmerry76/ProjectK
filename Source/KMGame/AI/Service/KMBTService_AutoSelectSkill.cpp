#include "KMBTService_AutoSelectSkill.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_SkillSet.h"

UKMBTService_AutoSelectSkill::UKMBTService_AutoSelectSkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "AutoSelectSkill";
	bNotifyTick = true;
}

FKMSkillKey UKMBTService_AutoSelectSkill::EvaluateBestSkill(const UKMCharacterInstance* casterCharacterInstance, const UKMCharacterInstance* targetCharacterInstance) const
{
	check(IsValid(casterCharacterInstance) && IsValid(targetCharacterInstance));

	UKMSkillHandler* ownerSkillHandler = casterCharacterInstance->GetSkillHandler();
	check(IsValid(ownerSkillHandler));

	const FKMTable_SkillSetRow* bestSkillSet = ownerSkillHandler->EvaluateSkillSet(targetCharacterInstance);
	if (!bestSkillSet)
	{
		return FKMSkillKey::Null();
	}
	
	if (bestSkillSet->Skills.IsEmpty())
	{
		return FKMSkillKey::Null();
	}

	return FKMSkillKey(bestSkillSet->Skills[0], 0);
}

void UKMBTService_AutoSelectSkill::OnSearchStartAI(AAIController* ownerController, APawn* controlledPawn)
{
	Super::OnSearchStartAI(ownerController, controlledPawn);	
}

void UKMBTService_AutoSelectSkill::TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	AAIController* aiController = ownerComp.GetAIOwner();
	check(IsValid(aiController));
	
	check(IsValid(aiController));
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character));

	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance));
	
/*	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem));*/

	UBlackboardComponent* blackboardComponent = aiController->GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	blackboardComponent->SetValueAsName(TEXT("SkillId"), NAME_None);
	blackboardComponent->SetValueAsInt(TEXT("SkillLevel"), 0);

	AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	if (IsValid(targetCharacter))
	{
		const FKMSkillKey skillKey = EvaluateBestSkill(ownerCharacterInstance, targetCharacter->GetCharacterInstance());
		if (skillKey.IsValid())
		{
			blackboardComponent->SetValueAsName(TEXT("SkillId"), skillKey.TableId);
			blackboardComponent->SetValueAsInt(TEXT("SkillLevel"), skillKey.Level);
		}
	}
}

UKMBTService_PerceptionTarget::UKMBTService_PerceptionTarget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "PerceptionTarget";
	bNotifyTick = true;
}

void UKMBTService_PerceptionTarget::OnSearchStartAI(AAIController* ownerController, APawn* controlledPawn)
{
	Super::OnSearchStartAI(ownerController, controlledPawn);
}

void UKMBTService_PerceptionTarget::TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	AAIController* aiController = ownerComp.GetAIOwner();
	check(IsValid(aiController));
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	if(!IsValid(character))
	{
		return;
	}

	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance));
	
	//	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	//check(IsValid(targetSubsystem));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));

	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	const UKMCharacterInstance* aggroTarget = ownerCharacterInstance->GetBestAggroTarget();
	if (IsValid(aggroTarget))
	{
		//if (blackboardComponent->GetValueAsObject(TEXT("TargetActor")) != aggroTarget->GetCharacter())
		{
			blackboardComponent->SetValueAsObject(TEXT("TargetActor"), nullptr);
			blackboardComponent->SetValueAsObject(TEXT("TargetActor"), aggroTarget->GetCharacter());
		}
	}
	else
	{
		blackboardComponent->SetValueAsObject(TEXT("TargetActor"), nullptr);
	}
}