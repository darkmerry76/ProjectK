#include "KMAnimNotifyState_GameplayTag.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"

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

void UKMAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		for (auto gameplayTag : TagContainer.GetGameplayTagArray())
		{
			ownerCharacter->GetCharacterInstance()->AddGameplayTag(gameplayTag);
		}
	}
}

void UKMAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		for (auto gameplayTag : TagContainer.GetGameplayTagArray())
		{
			ownerCharacter->GetCharacterInstance()->RemoveGameplayTag(gameplayTag);
		}
	}
}