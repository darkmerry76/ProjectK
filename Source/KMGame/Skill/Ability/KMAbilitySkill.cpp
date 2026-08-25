#include "KMAbilitySkill.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"
#include "Util/KMUtil.h"

UKMAbilitySkill::UKMAbilitySkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilitySkill::Activate()
{
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	PlayMartialArts(nullptr, Rate, bIsLoop);	
	PostActivated();

	Super::Activate();
}

void UKMAbilitySkill::PostActivated()
{
	if (bIsDirectionFallow && IsValid(GetTargetCharacter()))
	{
		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
			
		FVector targetToDirection = GetTargetCharacter()->GetActorLocation() - GetOwnerCharacter()->GetActorLocation();
		targetToDirection.Z = 0.0f;
		targetToDirection.Normalize();

		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * DirectionWeight), bIsForceRotation);
	}
}

void UKMAbilitySkill::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMSkillHandler* skillHandler = character->GetCharacterInstance()->GetSkillHandler();

	if (SkillInstance.IsValid() && !bCancel && EndingTag.IsValid())
	{
		skillHandler->TransitionTechniqueSkill(SkillInstance.Pin(), EndingTag);
	}

	if (bIsDirectionFallow)
	{
/*		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		FVector ownerForwardDirection = character->GetActorForwardVector();
		ownerForwardDirection.Z = 0.0f;
		ownerForwardDirection.Normalize();
	
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerForwardDirection)));*/
	}
}

void UKMAbilitySkill::SetSkillInstance(const TSharedPtr<FKMSkillInstance>& newSkillInstance)
{
	SkillInstance = newSkillInstance;
}

TWeakPtr<FKMSkillInstance> UKMAbilitySkill::GetSkillInstance() const
{
	return SkillInstance;
}

void UKMAbilitySkill::ForceComplate()
{
	if (SkillInstance.IsValid())
	{
		SkillInstance.Pin()->SetForceComplete(true);
	}
}