#include "KMAbilityCarry.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "GameActor/Pawn/Character/KMCharacter.h"

UKMAbilityCarry::UKMAbilityCarry(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityCarry::Activate()
{
	Super::Activate();
}

void UKMAbilityCarry::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
}

bool UKMAbilityCarry::StartCarried(FName attackSocket, float blendingDuration)
{
	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return false;
	}
	
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	if (!ownerPawnInterface)
	{
		return false;
	}

	AActor* targetActor = GetTargetActor();
	if (!IsValid(targetActor))
	{
		return false;
	}
	
	IKMPawnInterface* targetPawnInterface = Cast<IKMPawnInterface>(targetActor);
	if (!targetPawnInterface)
	{
		return false;
	}
	
	UKMAttachedBlendingComponent* targetAttachedBlendingComponent = targetPawnInterface->GetAttachedBlendingComponent();
	if (!IsValid(targetAttachedBlendingComponent))
	{
		return false;
	}
	
	FTransform targetWorldTransform = targetAttachedBlendingComponent->GetComponentToWorld();
	ownerPawnInterface->StartCrarry(targetPawnInterface->GetGameObjectInstance());
	targetPawnInterface->StartCrarried(ownerPawnInterface->GetGameObjectInstance());
	targetActor->AttachToComponent(ownerActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
	{
		targetAttachedBlendingComponent->StartBlending(ownerCharacter->GetMesh(), attackSocket, targetWorldTransform, blendingDuration);
	}
	return true;
}

void UKMAbilityCarry::CompleteCarried()
{
}