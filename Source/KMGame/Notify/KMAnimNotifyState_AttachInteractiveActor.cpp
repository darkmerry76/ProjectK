#include "KMAnimNotifyState_AttachInteractiveActor.h"

#include "Component/KMAttachedBlendingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameActor/Pawn/Interactive/KMInteractiveActorBase.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilitySkill.h"

bool FKMAnimNotifyState_AttachInteractiveActor_Context::IsValid() const
{
	return ::IsValid(InteractiveActor);
}

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

TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context> UKMAnimNotifyState_AttachInteractiveActor::GetContext(USkeletalMeshComponent* meshComp, const FAnimNotifyEventReference& eventReference) const
{
	const TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context>* existPairContext = Contexts.Find(meshComp);
	if (existPairContext && existPairContext->IsValid())
	{
		return *existPairContext;
	}

	AKMInteractiveActorBase* interactiveActor = nullptr;
	if (meshComp->GetWorld() && meshComp->GetWorld()->IsGameWorld())
	{
		if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
		{
			if (UKMAbilitySkill* AbilitySkill = Cast<UKMAbilitySkill>(martialArtsData->GetAbility()))
			{
				interactiveActor = Cast<AKMInteractiveActorBase>(AbilitySkill->GetTargetActor());
			}
		}
	}
#if WITH_EDITOR
	if (!IsValid(interactiveActor) && meshComp->GetWorld() && !meshComp->GetWorld()->IsGameWorld())
	{
		interactiveActor = Cast<AKMInteractiveActorBase>(meshComp->GetWorld()->SpawnActor(PreviewInteractiveActorClass));
		if (IsValid(interactiveActor))
		{
			float radius = interactiveActor->GetMasterBounds().BoxExtent.Size2D();
			interactiveActor->SetActorLocation(meshComp->GetOwner()->GetActorLocation() + meshComp->GetOwner()->GetActorForwardVector() * radius);
		}
	}
#endif
	
	TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context> newContext = MakeShared<FKMAnimNotifyState_AttachInteractiveActor_Context>();
	newContext->InteractiveActor = interactiveActor;

	Contexts.Emplace(meshComp, newContext);
	return newContext;
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetSkeletalMeshComponent(meshComp);
	if (!IsValid(targetMeshComp))
	{
		return;
	}

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(targetMeshComp->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context> context = GetContext(targetMeshComp, eventReference);
	if (!context.IsValid() || !context->IsValid())
	{
		return;
	}
	
	if (UKMAttachedBlendingComponent* attachedComponent = Cast<UKMAttachedBlendingComponent>(context->InteractiveActor->GetAttachedComponent()))
	{
		FTransform startTransform = attachedComponent->GetComponentToWorld();

		context->InteractiveActor->Crarried(ownerCharacter->GetCharacterInstance());
		context->InteractiveActor->AttachToComponent(ownerCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
		attachedComponent->StartBlending(ownerCharacter->GetMesh(), AttachSocket, startTransform, BlendingDuration);
	}
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_AttachInteractiveActor::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetSkeletalMeshComponent(meshComp);
	if (!IsValid(targetMeshComp))
	{
		return;
	}

#if WITH_EDITOR
	const TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context>* existPairContext = Contexts.Find(targetMeshComp);
	if (existPairContext && existPairContext->IsValid())
	{
		if (IsValid(targetMeshComp->GetWorld()) && !targetMeshComp->GetWorld()->IsGameWorld())
		{
			AKMInteractiveActorBase* interactiveActor = (*existPairContext)->InteractiveActor;
			if (IsValid(interactiveActor))
			{
				interactiveActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				interactiveActor->Destroy();
			}
		}
#endif
	}
	Contexts.Remove(targetMeshComp);
}