#include "KMAnimNotifyState_Flag.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"

UKMAnimNotifyState_Flag::UKMAnimNotifyState_Flag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Plan);
}

void UKMAnimNotifyState_Flag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		//ownerCharacter->GetCharacterInstance()->GetSkillHandler()->SetAbilityFlag(AbilityFlag);
	}
}

void UKMAnimNotifyState_Flag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		//ownerCharacter->GetCharacterInstance()->GetSkillHandler()->ClearAbilityFlag(AbilityFlag);
	}
}