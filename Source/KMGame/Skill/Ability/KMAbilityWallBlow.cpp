#include "KMAbilityWallBlow.h"
#include "EMCurveWarpingComponent.h"
#include "Core/KMGameplayTag.h"
#include "GameActor/Pawn/Character/KMPawnBase.h"
#include "Skill/KMSkillHandler.h"

UKMAbilityWallBlow::UKMAbilityWallBlow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityWallBlow::Activated()
{
	Super::Activated();
}

void UKMAbilityWallBlow::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode movementMode)
{
	AKMPawnBase* pawn = Cast<AKMPawnBase>(GetOwnerActor());
	check(IsValid(pawn));

	UKMSkillHandler* skillHandler = pawn->GetGameObjectInstance()->GetSkillHandler();
	if (IsValid(skillHandler))
	{
		switch(movementMode)
		{
		case EEMCustomMovementMode::CMODE_Falling: skillHandler->TriggerTransitionSkillEffect(SkillEffectInstance.Pin(), FKMGameplayTagName::Event_Bound_Wall_Tag); break; 
		default:break;
		}
	}
}

void UKMAbilityWallBlow::Impact(const FTransform& newImpactTransform)
{
	Super::Impact(newImpactTransform);
}

void UKMAbilityWallBlow::Deactivated(bool bCancel)
{
	Super::Deactivated(bCancel);
}