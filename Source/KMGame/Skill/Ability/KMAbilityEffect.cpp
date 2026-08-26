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
	CasterGameObject = Cast<UKMGameObjectInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(CastObjectKey));
	
	PlayMartialArts(nullptr, Rate, bIsLoop);
	PostActivated();
	
	Super::Activate();
}

void UKMAbilityEffect::PostActivated()
{
	if (bIsDirectionFallow)
	{
		if (UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance())
		{
			FVector targetToDirection = GetOwnerCharacter()->GetActorLocation() - GetCasterCharacter()->GetActorLocation();
			targetToDirection.Z = 0.0f;
			targetToDirection.Normalize();
			ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * DirectionWeight), bIsForceRotation);
		}
	}
}

void UKMAbilityEffect::Impact(const FTransform& newImpactTransform)
{
	if (UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance())
	{
		if (AKMCharacter* ownerCharacter = ownerCharacterInstance->GetCharacter())
		{
			FVector targetToDirection = ownerCharacter->GetActorLocation() - newImpactTransform.GetLocation();
			targetToDirection.Z = 0.0f;
			targetToDirection.Normalize();
			ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * -1.f));
		}
	}
	
	Super::Impact(newImpactTransform);
}

void UKMAbilityEffect::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
	
	UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance();
	check(IsValid(ownerGameObjectInstance));

	UKMSkillHandler* skillHandler = ownerGameObjectInstance->GetSkillHandler();
	check(IsValid(skillHandler));

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
	CasterGameObject = Cast<UKMGameObjectInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(CastObjectKey));
}

AActor* UKMAbilityEffect::GetCasterActor() const
{
	if(!CasterGameObject.IsValid())
	{
		return nullptr;
	}
	return CasterGameObject->GetOwnerActor();
}

AKMCharacter* UKMAbilityEffect::GetCasterCharacter() const
{
	return Cast<AKMCharacter>(GetCasterActor());
}

UKMGameObjectInstance* UKMAbilityEffect::GetCasterGameObjectInstance() const
{
	if(!CasterGameObject.IsValid())
	{
		return nullptr;
	}
	return CasterGameObject.Pin().Get();	
}

UKMCharacterInstance* UKMAbilityEffect::GetCasterCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetCasterGameObjectInstance());
}

void UKMAbilityEffect::SetSkillEffectInstance(const TSharedPtr<FKMSkillEffectInstance>& newSkillEffectInstance)
{
	SkillEffectInstance = newSkillEffectInstance;
}

FKMSkillEffectInstance* UKMAbilityEffect::GetSkillEffectInstance() const
{
	return SkillEffectInstance.IsValid() ? SkillEffectInstance.Pin().Get() : nullptr;
}