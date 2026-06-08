#include "KMAnimationSetTag.h"
#include "GameplayTagsManager.h"

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
