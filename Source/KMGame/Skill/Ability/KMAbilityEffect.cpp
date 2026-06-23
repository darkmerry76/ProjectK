#include "KMAbilityEffect.h"
#include "Animation/AnimSet/KMAnimationSetEffect.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMGameObjectSubsystem.h"
#include "Util/KMUtil.h"

UKMAbilityEffect::UKMAbilityEffect(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityEffect::Activate()
{
	CasterCharacterObject = Cast<UKMCharacterInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(CastObjectKey));
	
	Super::Activate();
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	if (animEffectType != EKMAnimSetEffectType::None || IsValid(Montage))
	{
		UKMAnimationSetEffect* animSetEffect = character->EffectAnimset;
		check(IsValid(animSetEffect));

		TObjectPtr<UAnimMontage>* existMontage = animSetEffect->AnimMontageMap.Find(animEffectType);
		if (existMontage && IsValid(*existMontage))
		{
			Montage = *existMontage;
		}
		MontageInstance = PlayerMontage(Montage, Rate);
	}
	else
	{
		PlayMartialArts(nullptr, Rate, false);
	}

	if (bIsDirectionFallow && IsValid(GetCasterCharacter()))
	{
		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		FVector targetToDirection = GetOwnerCharacter()->GetActorLocation() - GetCasterCharacter()->GetActorLocation();
		targetToDirection.Z = 0.0f;
		targetToDirection.Normalize();
		
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * -1.f));
	}
}

void UKMAbilityEffect::Deactivate()
{
	Super::Deactivate();
	
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMSkillHandler* skillHandler = character->GetCharacterInstance()->GetSkillHandler();

	skillHandler->TriggerTransitionSkillEffect(EndingTag);

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