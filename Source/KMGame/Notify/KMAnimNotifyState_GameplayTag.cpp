#include "KMAnimNotifyState_GameplayTag.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"

UKMAnimNotifyState_GameplayTag::UKMAnimNotifyState_GameplayTag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{

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