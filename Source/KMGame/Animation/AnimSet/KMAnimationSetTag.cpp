#include "KMAnimationSetTag.h"
#include "GameplayTagsManager.h"
#include "Core/KMGameplayTag.h"

UKMAnimationSetTag::UKMAnimationSetTag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	FGameplayTag rootTag = FGameplayTag::RequestGameplayTag(TEXT("Anim"));
	FGameplayTagContainer tagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(rootTag);

	for (const FGameplayTag& tag : tagContainer)
	{
		if (UGameplayTagsManager::Get().RequestGameplayTagChildren(tag).IsEmpty())
		{
			AnimMontageMap.Emplace(tag, nullptr);
		}
	}
}

UAnimMontage* UKMAnimationSetTag::GetAnimation(FGameplayTag tag) const
{
	const TObjectPtr<UAnimMontage>* existOverrideMontage = AnimOverrideMontageMap.Find(tag);
	if (existOverrideMontage && IsValid(*existOverrideMontage))
	{
		return *existOverrideMontage;
	}
	
	const TObjectPtr<UAnimMontage>* existMontage = AnimMontageMap.Find(tag);
	if (!existMontage)
	{
		return nullptr;
	}
	return *existMontage;
}

void UKMAnimationSetTag::SetMovementOverrideMontage(UAnimMontage* jumpMontage, UAnimMontage* landingMontage)
{
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_0, jumpMontage);
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_0, landingMontage);

	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_1, jumpMontage);
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_1, landingMontage);

	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_2, jumpMontage);
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_2, landingMontage);
}

void UKMAnimationSetTag::RemoveMovementOverrideMontage()
{
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Jump_0);
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Landing_0);
	
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Jump_1);
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Landing_1);
	
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Jump_2);
	AnimOverrideMontageMap.Remove(FKMGameplayTagName::Anim_Jump_2);
}