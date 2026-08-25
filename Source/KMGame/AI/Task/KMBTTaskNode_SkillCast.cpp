#include "KMBTTaskNode_SkillCast.h"
#include "AIController.h"
#include "AI/KMAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Navigation/PathFollowingComponent.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMTargetSubsystem.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_SkillCast::UKMBTTaskNode_SkillCast(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "SkillCast";
	//bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_SkillCast::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));
	
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));
	
//	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
//	check(IsValid(targetSubsystem));

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(TEXT("sk_hugo_gigas"), 0);
	
	if (!ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag) &&
		skillHandler->IsSkillAvailable(skillKey))
	{
		if (!ownerCharacterInstance->UseSkill(skillKey.TableId, 0))
		{
			//return EBTNodeResult::Failed;
		}
	}

	return EBTNodeResult::Succeeded;
}

void UKMBTTaskNode_SkillCast::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));
	
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(TEXT("sk_hugo_gigas"), 0);
	if (!skillHandler->IsSkillAvailable(skillKey))
	{
		FinishLatentTask(ownerComp, EBTNodeResult::Succeeded);
	}
}

UKMBTTaskNode_Run::UKMBTTaskNode_Run(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "Run";
//	bNotifyTick = true;
}

EBTNodeResult::Type UKMBTTaskNode_Run::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	AKMCharacter* ownerCharacter = ownerCharacterInstance->GetCharacter();
	check(IsValid(ownerCharacter));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));
	
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	if (ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		return EBTNodeResult::Succeeded;
	}

	if (AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor"))))
	{
		AKMAIController* aIController = Cast<AKMAIController>(ownerCharacter->GetController());
		if (IsValid(aIController))
		{
			FVector targetLocation = targetCharacter->GetActorLocation();
			
			FAIMoveRequest moveRequest;
			moveRequest.SetGoalLocation(targetLocation);
			moveRequest.SetAcceptanceRadius(50.f);

			FNavPathSharedPtr navPath;
			aIController->MoveTo(moveRequest, &navPath);
			ownerCharacterInstance->Run();
		}
		
		FVector directional = targetCharacter->GetActorLocation() - ownerCharacterInstance->GetCharacter()->GetActorLocation();
		directional.Y = 0.f;
		directional.Z = 0.f;
		if (directional.Size() > 150.f)
		{
			directional.Normalize();
		
//			ownerCharacterInstance->SetCharacterDirection(directional.X);
//			ownerCharacterInstance->Run();
		}
		else
		{
		}
	
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Aborted;
}

void UKMBTTaskNode_Run::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));

	UKMSkillHandler* skillHandler = ownerCharacterInstance->GetSkillHandler();
	check(IsValid(skillHandler));
	
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	float targetToDistance = 0.f;
	
	AKMCharacter* targetCharacter = Cast<AKMCharacter>(blackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	if (IsValid(targetCharacter))
	{
		FVector directional = targetCharacter->GetActorLocation() - ownerCharacterInstance->GetCharacter()->GetActorLocation();
		directional.Y = 0.f;
		directional.Z = 0.f;
		targetToDistance = directional.Size();
	}
	
	if (ownerCharacterInstance->IsDead() || !IsValid(targetCharacter) || targetToDistance < 150.f)
	{
		FinishLatentTask(ownerComp, EBTNodeResult::Succeeded);
	}
}