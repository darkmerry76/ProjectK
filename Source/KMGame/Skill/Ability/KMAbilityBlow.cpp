#include "KMAbilityBlow.h"
#include "EMCurveWarpingComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Core/KMGameplayTag.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"

UKMAbilityBlow::UKMAbilityBlow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityBlow::Activate()
{
	Super::Activate();
}

void UKMAbilityBlow::Trigger(const FGameplayTag& eventTag)
{
	Super::Trigger(eventTag);

	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UKMSkillHandler* skillHandler = ownerCharacter->GetCharacterInstance()->GetSkillHandler();
	if (!IsValid(skillHandler))
	{
		return;
	}

	if (eventTag == FKMGameplayTagName::Event_Move_Landing_Tag)
	{
		skillHandler->TriggerTransitionSkillEffect(FGameplayTag::RequestGameplayTag(TEXT("event.blow.landing")));
	}
}

void UKMAbilityBlow::Impact(const FTransform& newImpactTransform)
{
	Super::Impact(newImpactTransform);
	
	OnImpact(newImpactTransform);
}

void UKMAbilityBlow::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
	
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if(!IsValid(ownerCharacter))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarpingComponent();
	check(IsValid(curveWarping));

	curveWarping->GetInteruptDelegate().RemoveAll(this);
}