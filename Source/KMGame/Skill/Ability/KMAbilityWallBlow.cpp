#include "KMAbilityWallBlow.h"
#include "EMCurveWarpingComponent.h"
#include "Core/KMGameplayTag.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Skill/KMSkillHandler.h"

UKMAbilityWallBlow::UKMAbilityWallBlow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityWallBlow::Activate()
{
	Super::Activate();
}

void UKMAbilityWallBlow::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode movementMode)
{
	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	check(pawnInterface);

	UKMSkillHandler* skillHandler = pawnInterface->GetGameObjectInstance()->GetSkillHandler();
	if (IsValid(skillHandler))
	{
		switch(movementMode)
		{
		case EEMCustomMovementMode::CMODE_Falling: skillHandler->TriggerTransitionSkillEffect(FKMGameplayTagName::Event_Bound_Wall_Tag); break; 
		default:break;
		}
	}
}

void UKMAbilityWallBlow::Impact(const FTransform& newImpactTransform)
{
	Super::Impact(newImpactTransform);
}

void UKMAbilityWallBlow::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
}