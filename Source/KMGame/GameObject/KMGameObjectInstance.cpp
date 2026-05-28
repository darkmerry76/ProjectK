#include "KMGameObjectInstance.h"

UKMGameObjectInstance::UKMGameObjectInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMGameObjectInstance::BeginPlay()
{
	Super::BeginPlay();
}

void UKMGameObjectInstance::EndPlay()
{
	Super::EndPlay();
}

void UKMGameObjectInstance::AddGameplayTag(FGameplayTag newTag)
{
	GameplayTagContainer.AddTag(newTag);

	OnAddGameplayTag(newTag);
}

void UKMGameObjectInstance::OnAddGameplayTag_Implementation(const FGameplayTag& newTag)
{
}

void UKMGameObjectInstance::RemoveGameplayTag(FGameplayTag removedTag)
{
	GameplayTagContainer.RemoveTag(removedTag);

	OnRemoveGameplayTag(removedTag);
}

void UKMGameObjectInstance::OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag)
{
}


bool UKMGameObjectInstance::HasGameplayTag(FGameplayTag tag) const
{
	return GameplayTagContainer.HasTag(tag);
}

int32 UKMGameObjectInstance::GetGameplayTagCount(FGameplayTag tag) const
{
	return GameplayTagContainer.GetRefCount(tag);
}