#include "KMAbilitySkill.h"
#include "Animation/AnimSet/KMAnimationSetSkill.h"
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

	UKMAnimationSetSkill* animSetSkill = character->SkillAnimset;
	check(IsValid(animSetSkill));

	TObjectPtr<UAnimMontage>* existMontage = animSetSkill->AnimMontageMap.Find(AnimSkillType);
	if (existMontage && IsValid(*existMontage))
	{
		Montage = *existMontage;
	}

	MontageInstance = PlayerMontage(Montage, Rate);

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
		if (IsValid(Montage))
		{
			StopMontage(Montage);
		}
	}
	if (bIsDirectionFallow)
	{
		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		FVector ownerForwardDirection = GetOwnerCharacter()->GetActorForwardVector();
		ownerForwardDirection.Z = 0.0f;
		ownerForwardDirection.Normalize();
	
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerForwardDirection)));
	}
}