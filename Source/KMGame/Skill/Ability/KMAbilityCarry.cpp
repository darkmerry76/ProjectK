#include "KMAbilityCarry.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"

UKMAbilityCarry::UKMAbilityCarry(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityCarry::Activated()
{
	Super::Activated();
}

void UKMAbilityCarry::Deactivated(bool bCancel)
{
	Super::Deactivated(bCancel);
}

bool UKMAbilityCarry::StartCarried(FName attackSocket, float blendingDuration)
{
	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return false;
	}
	
	AKMPawnBase* ownerPawn = Cast<AKMPawnBase>(GetOwnerActor());
	if (!IsValid(ownerPawn))
	{
		return false;
	}

	AActor* targetActor = GetTargetActor();
	if (!IsValid(targetActor))
	{
		return false;
	}
	
	AKMPawnBase* targetPawn = Cast<AKMPawnBase>(targetActor);
	if (!IsValid(targetPawn))
	{
		return false;
	}
	
	UKMAttachedBlendingComponent* targetAttachedBlendingComponent = targetPawn->GetAttachedBlendingComponent();
	if (!IsValid(targetAttachedBlendingComponent))
	{
		return false;
	}
	
	FTransform targetWorldTransform = targetAttachedBlendingComponent->GetComponentToWorld();
	if (UKMGameObjectInstance* targetGameObjectInstance = GetTargetGameObjectInstance())
	{
		targetGameObjectInstance->StartCrarried(ownerPawn->GetGameObjectInstance());
	}
	targetActor->AttachToComponent(ownerActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
	{
		targetWorldTransform.SetScale3D(FVector::One());
		targetAttachedBlendingComponent->StartBlending(ownerCharacter->GetMesh(), attackSocket, targetWorldTransform, blendingDuration);
	}
	return true;
}

void UKMAbilityCarry::CompleteCarried()
{
}