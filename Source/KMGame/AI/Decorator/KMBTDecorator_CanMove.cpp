#include "KMBTDecorator_CanMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMMonsterInstance.h"

UKMBTDecorator_CanMove::UKMBTDecorator_CanMove(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = TEXT("CanMove");
}

bool UKMBTDecorator_CanMove::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
{
	AAIController* aiController = ownerComp.GetAIOwner();
	check(IsValid(aiController));
	
	AKMCharacter* character = Cast<AKMCharacter>(aiController->GetPawn());
	check(IsValid(character));
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(character->GetCharacterInstance());
	check(IsValid(ownerCharacterInstance));

	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));
	
	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));
	if (!skillKey.IsValid())
	{
		return false;
	}
	
	//return ownerCharacterInstance->CanMove();
	return false;
}