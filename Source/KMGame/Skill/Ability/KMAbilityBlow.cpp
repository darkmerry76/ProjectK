#include "KMAbilityBlow.h"
#include "EMCurveWarpingComponent.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Skill/KMSkillHandler.h"

UKMAbilityBlow::UKMAbilityBlow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityBlow::Activate()
{
	Super::Activate();
}

void UKMAbilityBlow::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType type)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UKMSkillHandler* skillHandler = ownerCharacter->GetCharacterInstance()->GetSkillHandler();
	switch(type)
	{
		case EEMCurveWarpingInteruptType::Landing:
			skillHandler->TriggerTransitionSkillEffect(FGameplayTag::RequestGameplayTag(TEXT("event.blow.landing"))); break; 
		default:break;
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
	check(IsValid(ownerCharacter));

	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	check(IsValid(curveWarping));

	curveWarping->GetInteruptDelegate().RemoveAll(this);
}