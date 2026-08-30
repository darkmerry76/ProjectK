#include "KMAnimNotifyState_GameplayTag.h"

#include "GameActor/Pawn/Character/KMCharacter.h"

UKMAnimNotifyState_GameplayTag::UKMAnimNotifyState_GameplayTag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Plan);
}

FString UKMAnimNotifyState_GameplayTag::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (!TagContainer.IsEmpty())
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *TagContainer.ToStringSimple());		
	}
	return notifyName;
}

void UKMAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner()))
	{
		for (auto gameplayTag : TagContainer.GetGameplayTagArray())
		{
			if (UKMGameObjectInstance* ownerGameObjectInstance = ownerCharacter->GetCharacterInstance())
			{
				ownerGameObjectInstance->AddGameplayTag(gameplayTag);
			}
		}
	}
}

void UKMAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner()))
	{
		for (auto gameplayTag : TagContainer.GetGameplayTagArray())
		{
			if (UKMGameObjectInstance* ownerGameObjectInstance = ownerCharacter->GetCharacterInstance())
			{
				ownerGameObjectInstance->RemoveGameplayTag(gameplayTag);
			}
		}
	}
}