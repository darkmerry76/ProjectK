#include "KMSoundSetTag.h"
#include "GameplayTagsManager.h"

UKMSoundSetTag::UKMSoundSetTag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	FGameplayTag rootTag = FGameplayTag::RequestGameplayTag(TEXT("Sound"));
	FGameplayTagContainer tagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(rootTag);

	for (const FGameplayTag& tag : tagContainer)
	{
		if (UGameplayTagsManager::Get().RequestGameplayTagChildren(tag).IsEmpty())
		{
			SoundMap.Emplace(tag, nullptr);
		}
	}
}

USoundBase* UKMSoundSetTag::GetSound(FGameplayTag tag) const
{
	const TObjectPtr<USoundBase>* existSound = SoundMap.Find(tag);
	if (!existSound)
	{
		return nullptr;
	}
	return *existSound;
}