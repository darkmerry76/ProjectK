#include "KMBTDecorator_CanMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "GameObject/KMMonsterInstance.h"
#include "Skill/KMSkillTypes.h"

UKMBTDecorator_CanMove::UKMBTDecorator_CanMove(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanMove");
}

bool UKMBTDecorator_CanMove::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* aiController = OwnerComp.GetAIOwner();
	check(IsValid(aiController) == true);
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character) == true);
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance) == true);

	UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent) == true);
	
	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));
	if (skillKey.IsValid() == false)
	{
		return false;
	}
	
	//return ownerCharacterInstance->CanMove();
	return false;
}