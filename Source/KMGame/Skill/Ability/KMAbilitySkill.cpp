#include "KMAbilitySkill.h"
#include "Character/KMCharacter.h"
#include "Util/KMUtil.h"

UKMAbilitySkill::UKMAbilitySkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilitySkill::Activate()
{
	Super::Activate();
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	PlayMartialArts(nullptr, Rate, bIsLoop);

	if (bIsDirectionFallow && IsValid(GetTargetCharacter()))
	{
		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
			
		FVector targetToDirection = GetTargetCharacter()->GetActorLocation() - GetOwnerCharacter()->GetActorLocation();
		targetToDirection.Z = 0.0f;
		targetToDirection.Normalize();

		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection)));
	}
}

void UKMAbilitySkill::Deactivate()
{
	Super::Deactivate();
	
	AKMCharacter* character = GetOwnerCharacter();
	if (IsValid(character))
	{
		if (bIsDirectionFallow)
		{
/*			UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
			check(IsValid(ownerCharacterInstance));
			
			FVector ownerForwardDirection = character->GetActorForwardVector();
			ownerForwardDirection.Z = 0.0f;
			ownerForwardDirection.Normalize();
		
			ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerForwardDirection)));*/
		}
	}
}