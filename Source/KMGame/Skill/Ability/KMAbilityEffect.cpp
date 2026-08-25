#include "KMAbilityEffect.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMGameObjectSubsystem.h"
#include "Util/KMUtil.h"

UKMAbilityEffect::UKMAbilityEffect(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityEffect::Activate()
{
	CasterCharacterObject = Cast<UKMCharacterInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(CastObjectKey));
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	PlayMartialArts(nullptr, Rate, bIsLoop);
	PostActivated();
	Super::Activate();
}

void UKMAbilityEffect::PostActivated()
{
	if (bIsDirectionFallow && IsValid(GetCasterCharacter()))
	{
		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));

		FVector targetToDirection = GetOwnerCharacter()->GetActorLocation() - GetCasterCharacter()->GetActorLocation();
		targetToDirection.Z = 0.0f;
		targetToDirection.Normalize();
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * DirectionWeight), bIsForceRotation);
	}
}

void UKMAbilityEffect::Impact(const FTransform& newImpactTransform)
{
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
	check(IsValid(ownerCharacterInstance));

	FVector targetToDirection = character->GetActorLocation() - newImpactTransform.GetLocation();
	targetToDirection.Z = 0.0f;
	targetToDirection.Normalize();
	ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * -1.f));
	
	Super::Impact(newImpactTransform);
}

void UKMAbilityEffect::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMSkillHandler* skillHandler = character->GetCharacterInstance()->GetSkillHandler();

	if (SkillEffectInstance.IsValid() && !bCancel && EndingTag.IsValid())
	{
		skillHandler->TriggerTransitionSkillEffect(EndingTag);
	}

	if (bIsDirectionFallow)
	{
/*		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		FVector ownerForwardDirection = GetOwnerCharacter()->GetActorForwardVector();
		ownerForwardDirection.Z = 0.0f;
		ownerForwardDirection.Normalize();
	
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerForwardDirection)), true);*/
	}
}

void UKMAbilityEffect::SetCastObjectKey(FKMObjectKey newCasterObjectKey)
{
	CastObjectKey = newCasterObjectKey;
	CasterCharacterObject = Cast<UKMCharacterInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(CastObjectKey));
}

AKMCharacter* UKMAbilityEffect::GetCasterCharacter() const
{
	if(!CasterCharacterObject.IsValid())
	{
		return nullptr;
	}
	return CasterCharacterObject->GetCharacter();
}

UKMCharacterInstance* UKMAbilityEffect::GetCasterCharacterInstance() const
{
	if(!CasterCharacterObject.IsValid())
	{
		return nullptr;
	}
	return CasterCharacterObject.Pin().Get();
}

void UKMAbilityEffect::SetSkillEffectInstance(const TSharedPtr<FKMSkillEffectInstance>& newSkillEffectInstance)
{
	SkillEffectInstance = newSkillEffectInstance;
}

FKMSkillEffectInstance* UKMAbilityEffect::GetSkillEffectInstance() const
{
	return SkillEffectInstance.IsValid() ? SkillEffectInstance.Pin().Get() : nullptr;
}