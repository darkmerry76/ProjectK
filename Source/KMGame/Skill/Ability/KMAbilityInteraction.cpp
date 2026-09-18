#include "KMAbilityInteraction.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Skill/KMSkillHandler.h"

UKMAbilityInteraction::UKMAbilityInteraction(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityInteraction::Activate()
{
	Super::Activate();
	PostActivated();
}

void UKMAbilityInteraction::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
}

void UKMAbilityInteraction::OnRequestEnd_Implementation()
{
}

AActor* UKMAbilityInteraction::GetTargetActor() const
{
	UKMGameObjectInstance* targetGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(targetGameObjectInstance))
	{
		return nullptr;
	}

	return targetGameObjectInstance->GetOwnerActor();
}

UKMGameObjectInstance* UKMAbilityInteraction::GetTargetGameObjectInstance() const
{
	UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return nullptr;
	}

	return ownerGameObjectInstance->GetCarriedGameObjectInstance();
}

bool UKMAbilityInteraction::StartInteraction(FName attackSocket, float blendingDuration)
{
	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return false;
	}
	
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(ownerActor);
	if (!ownerPawnInterface)
	{
		return false;
	}

	UKMGameObjectInstance* carriedGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(carriedGameObjectInstance))
	{
		return false;
	}

	AActor* carriedActor = carriedGameObjectInstance->GetOwnerActor();
	if (!IsValid(carriedActor))
	{
		return false;
	}
	
	IKMPawnInterface* carriedPawnInterface = Cast<IKMPawnInterface>(carriedActor);
	if (!carriedPawnInterface)
	{
		return false;
	}
	
	FTransform carriedWorldTransform = carriedActor->GetActorTransform();
	carriedWorldTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
	{
		if (UKMAttachedBlendingComponent* carriedAttachedBlendingComponent = carriedPawnInterface->GetAttachedBlendingComponent())
		{
			//carriedAttachedBlendingComponent->StartBlending(ownerCharacter->GetMesh(), attackSocket, carriedWorldTransform, 0.f);
			carriedAttachedBlendingComponent->StopBlending();
			GetOwnerGameObjectInstance()->GetSkillHandler()->TriggerEvent(FKMGameplayTagName::Event_PutDown_Tag);
		}
	}

	carriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	carriedGameObjectInstance->ComplatePutdowned(GetOwnerGameObjectInstance());
	carriedPawnInterface->GetMovementShapeComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	OnStartInteraction();
	return true;
}