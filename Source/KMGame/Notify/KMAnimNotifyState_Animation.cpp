#include "KMAnimNotifyState_Animation.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "Character/KMCharacter.h"

UKMAnimNotifyState_Animation::UKMAnimNotifyState_Animation(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_Animation::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(Montage))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *Montage->GetName());		
	}
	
	return notifyName;
}

UAnimMontage* UKMAnimNotifyState_Animation::GetUsedMontage(AActor* actor)
{
	if (bUseSkillSet)
	{
		AKMCharacter* ownerCharacter = Cast<AKMCharacter>(actor);
		if (!IsValid(ownerCharacter))
		{
			return nullptr;
		}
		if (!IsValid(ownerCharacter))
		{
			return nullptr;
		}
		if (!IsValid(ownerCharacter->AnimsetTag))
		{
			return nullptr;
		}
		
		TObjectPtr<UAnimMontage>* existMontage = ownerCharacter->AnimsetTag->AnimMontageMap.Find(AnimationSetTag.Tag);
		if (!existMontage)
		{
			return nullptr;
		}
		return *existMontage;
	}
	else
	{
		return Montage;
	}
}

bool UKMAnimNotifyState_Animation::IsCustomDuration() const
{
	return bUseSkillSet && CustomDuration > 0.f;
}

float UKMAnimNotifyState_Animation::GetCustomDuration() const
{
	return CustomDuration;
}

void UKMAnimNotifyState_Animation::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	TSharedPtr<FKMAnimNotifyState_Animation_Context> newContext = MakeShared<FKMAnimNotifyState_Animation_Context>();
	newContext->ActivatedMontage = GetUsedMontage(meshComp->GetOwner());

	Context.Emplace(meshComp, newContext);

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if (IsValid(newContext->ActivatedMontage) && IsValid(ownerCharacter))
	{
		CustomDuration = newContext->ActivatedMontage->GetPlayLength();

		if (UAnimInstance* animInstance = meshComp->GetAnimInstance())
		{
			animInstance->Montage_Play(newContext->ActivatedMontage);
			animInstance->Montage_Pause(newContext->ActivatedMontage);
		}
	}
}

void UKMAnimNotifyState_Animation::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(meshComp);
	if (currContext)
	{
		(*currContext)->ElapsedTime += frameDeltaTime;
	}
}

void UKMAnimNotifyState_Animation::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(meshComp);
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if (currContext && currContext->IsValid() && IsValid((*currContext)->ActivatedMontage) && IsValid(ownerCharacter))
	{
		ownerCharacter->StopAnimMontage((*currContext)->ActivatedMontage);
	}
	Context.Remove(meshComp);
}

#if WITH_EDITOR

void UKMAnimNotifyState_Animation::SetEditorPosition(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float currentTime, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(meshComp))
	{
		if (UAnimInstance* animInstance = meshComp->GetAnimInstance())
		{
			if (FAnimMontageInstance* montageInstance = animInstance->GetActiveInstanceForMontage((*currContext)->ActivatedMontage))
			{
				float previousPosition = montageInstance->GetPosition();
				montageInstance->SetPosition(currentTime);
				montageInstance->HandleEvents(previousPosition, currentTime, nullptr);
				animInstance->TriggerAnimNotifies(currentTime - previousPosition);
			}
		}
	}
}

void UKMAnimNotifyState_Animation::PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(ownerActor, propertyChangedEvent);
	if (propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotifyState_Animation, AnimationSetTag) ||
		propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotifyState_Animation, bUseSkillSet))
	{
		if (bUseSkillSet)
		{
			if (AKMCharacter* character = Cast<AKMCharacter>(ownerActor))
			{
				if (character->AnimsetTag)
				{
					if (TObjectPtr<UAnimMontage>* existMontage = character->AnimsetTag->AnimMontageMap.Find(AnimationSetTag.Tag))
					{
						Montage = *existMontage;
						CustomDuration = Montage->GetPlayLength();
					}
				}
			}
		}
	}
}

#endif