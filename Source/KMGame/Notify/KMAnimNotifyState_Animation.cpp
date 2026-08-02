#include "KMAnimNotifyState_Animation.h"
#include "EMMartialArts.h"
#include "Animation/KMAnimInstance.h"
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

UAnimMontage* UKMAnimNotifyState_Animation::GetUsedMontage(AActor* actor) const
{
	if (bIsUseSkillSet)
	{
		AKMCharacter* ownerCharacter = Cast<AKMCharacter>(actor);
		if (!IsValid(ownerCharacter))
		{
			return nullptr;
		}
		return ownerCharacter->GetAnimationTag(AnimationSetTag.Tag);
	}
	else
	{
		return Montage;
	}
}

bool UKMAnimNotifyState_Animation::IsCustomDuration() const
{
	return false;//bIsUseSkillSet && CustomDuration > 0.f;
}

float UKMAnimNotifyState_Animation::GetCustomDuration() const
{
	return CustomDuration;
}

void UKMAnimNotifyState_Animation::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(targetMeshComp->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}

	if (!IsCompareTag(ownerCharacterInstance))
	{
		return;
	}
	
	TSharedPtr<FKMAnimNotifyState_Animation_Context> newContext = MakeShared<FKMAnimNotifyState_Animation_Context>();
	newContext->ActivatedMontage = GetUsedMontage(targetMeshComp->GetOwner());

	if (eventReference.GetNotify())
	{
		CollectionMontageSection(meshComp, *eventReference.GetNotify());
	}

	Context.Emplace(targetMeshComp, newContext);
	
	if (IsValid(newContext->ActivatedMontage) && IsValid(ownerCharacter))
	{
		if (bIsOverrideMovementAnimSet)
		{
			ownerCharacter->SetMovementOverrideMontage(newContext->ActivatedMontage, newContext->ActivatedMontage);
		}

		UAnimInstance* targetAnimInstance = targetMeshComp->GetAnimInstance();
		if (IsValid(targetAnimInstance))
		{
			CustomDuration = newContext->ActivatedMontage->GetPlayLength();
			if (bIsEqualsPlay)
			{
				targetAnimInstance->Montage_Play(newContext->ActivatedMontage);
			}
			else
			{
				bool isActivatedMontage = false;
				for (auto mointageInstance : targetAnimInstance->MontageInstances)
				{
					if (mointageInstance->Montage == newContext->ActivatedMontage)
					{
						isActivatedMontage = true;
						break;
					}
				}
				if (!isActivatedMontage)
				{
					targetAnimInstance->Montage_Play(newContext->ActivatedMontage);
				}
			}
#if WITH_EDITOR
			if (!targetMeshComp->GetWorld()->IsGameWorld())
			{
				targetAnimInstance->Montage_Pause(newContext->ActivatedMontage);
			}
#endif
		}
	}

	if (bIsStartShowMeshComponent)
	{
		targetMeshComp->SetRenderInMainPass(true);
		targetMeshComp->SetRenderInDepthPass(true);

		if (UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(targetMeshComp->GetAnimInstance()))
		{
			animInstance->UpdateAnimation(0.f, true);
			targetMeshComp->RefreshBoneTransforms();
		}
	}
}

void UKMAnimNotifyState_Animation::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(targetMeshComp);
	if (currContext)
	{
		(*currContext)->ElapsedTime += frameDeltaTime;
	}
}

void UKMAnimNotifyState_Animation::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(targetMeshComp);
	if (!currContext)
	{
		return;
	}
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(targetMeshComp->GetOwner());
	if (IsValid(ownerCharacter))
	{
		if (bIsOverrideMovementAnimSet && IsValid((*currContext)->ActivatedMontage))
		{
			ownerCharacter->RemoveMovementOverrideMontage();
		}

		if (!bIsImmediate && currContext && currContext->IsValid() && IsValid((*currContext)->ActivatedMontage) && IsValid(ownerCharacter))
		{
			if (UAnimInstance* targetAnimInstance = targetMeshComp->GetAnimInstance())
			{
				targetAnimInstance->Montage_Stop(0.f, (*currContext)->ActivatedMontage);
			}
		}

		if (meshComp->GetWorld()->IsGameWorld())
		{
			if (bIsEndRemoveTagMeshComponent && targetMeshComp != meshComp)
			{
				if (IsValid(targetMeshComp))
				{
					targetMeshComp->DestroyComponent(true);
				}
			}
		}
	}
	Context.Remove(targetMeshComp);
}

#if WITH_EDITOR

void UKMAnimNotifyState_Animation::SetEditorPosition(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float currentTime, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	if (TSharedPtr<FKMAnimNotifyState_Animation_Context>* currContext = Context.Find(targetMeshComp))
	{
		if (UAnimInstance* animInstance = targetMeshComp->GetAnimInstance())
		{
			if (FAnimMontageInstance* montageInstance = animInstance->GetActiveInstanceForMontage((*currContext)->ActivatedMontage))
			{
				float previousPosition = montageInstance->GetPosition();
				float deltaTime = currentTime - previousPosition;
				
				montageInstance->SetPosition(currentTime);
				montageInstance->UpdateWeight(0.f);
				montageInstance->HandleEvents(previousPosition, currentTime, nullptr);
				animInstance->TriggerAnimNotifies(deltaTime);
			}
		}
	}
}

void UKMAnimNotifyState_Animation::PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(ownerActor, propertyChangedEvent);
	if (propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotifyState_Animation, AnimationSetTag) ||
		propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotifyState_Animation, bIsUseSkillSet))
	{
		if (bIsUseSkillSet)
		{
			if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
			{
				UKMCharacterInstance* ownerCharacteInstancer = Cast<UKMCharacterInstance>(ownerCharacter->GetCharacterInstance());
				if (!IsValid(ownerCharacteInstancer))
				{
					Montage = ownerCharacter->GetAnimationTag(AnimationSetTag.Tag);
					if (IsValid(Montage))
					{
						CustomDuration = Montage->GetPlayLength();
					}
				}
			}
		}
	}
}

void UKMAnimNotifyState_Animation::DrawInEditor(FPrimitiveDrawInterface* pDI, USkeletalMeshComponent* meshComp, const UAnimSequenceBase* animation, const FAnimNotifyEvent& notifyEvent) const
{
	Super::DrawInEditor(pDI, meshComp, animation, notifyEvent);

	const_cast<UKMAnimNotifyState_Animation*>(this)->CollectionMontageSection(meshComp, notifyEvent);
}

void UKMAnimNotifyState_Animation::CollectionMontageSection(USkeletalMeshComponent* meshComp, const FAnimNotifyEvent& notifyEvent)
{
	if (!bIsMaster)
	{
		return;
	}
	if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(GetContainingAsset()))
	{
		if (UAnimMontage* useMontage = GetUsedMontage(meshComp->GetOwner()))
		{
			martialArts->MontageSections.Empty();
			for (int32 sectionIndex = 0; sectionIndex < useMontage->GetNumSections(); ++sectionIndex)
			{
				const FCompositeSection& section = useMontage->GetAnimCompositeSection(sectionIndex);
				const float startTime = section.GetTime();

				float endTime = useMontage->GetPlayLength();

				if (sectionIndex + 1 < useMontage->GetNumSections())
				{
					endTime = useMontage->GetAnimCompositeSection(sectionIndex + 1).GetTime();
				}
				FEMMartialArtsMontageSection newMartialArtsSection;
				newMartialArtsSection.Name = section.SectionName;
				newMartialArtsSection.StartTime = notifyEvent.GetTime() + startTime;
				newMartialArtsSection.EndTime = notifyEvent.GetTime() + endTime;
				newMartialArtsSection.NextSectionIndex = useMontage->GetSectionIndex(section.NextSectionName);
				martialArts->MontageSections.Emplace(newMartialArtsSection);	
			}
		}
	}
}
#endif