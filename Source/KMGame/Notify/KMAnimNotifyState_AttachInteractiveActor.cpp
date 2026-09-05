#include "KMAnimNotifyState_AttachInteractiveActor.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilityCarry.h"
#include "Skill/Ability/KMAbilityPutDown.h"

UKMAnimNotifyState_AttachInteractiveActor::UKMAnimNotifyState_AttachInteractiveActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_AttachInteractiveActor::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	return notifyName;
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilityCarry* abilityCarry = Cast<UKMAbilityCarry>(martialArtsData->GetAbility()))
		{
			abilityCarry->StartCarried(AttachSocket, totalDuration);
		}
		else if (UKMAbilityPutDown* abilityPutDown = Cast<UKMAbilityPutDown>(martialArtsData->GetAbility()))
		{
			abilityPutDown->StartPutDown(AttachSocket, totalDuration);
		}
	}
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilityCarry* abilityCarry = Cast<UKMAbilityCarry>(martialArtsData->GetAbility()))
		{
			abilityCarry->CompleteCarried();
		}
		else if (UKMAbilityPutDown* abilityPutDown = Cast<UKMAbilityPutDown>(martialArtsData->GetAbility()))
		{
			abilityPutDown->CompletePutDown();
		}
	}
}