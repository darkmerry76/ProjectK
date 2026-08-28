#include "KMInteractiveInstance.h"

#include "GameActor/Pawn/Interactive/KMInteractiveActorBase.h"

UKMInteractiveInstance::UKMInteractiveInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMInteractiveInstance::BeginPlay()
{
	Super::BeginPlay();
}

void UKMInteractiveInstance::EndPlay()
{
	Super::EndPlay();
	
	if (Interactive.IsValid())
	{
		Interactive->Destroy();
		Interactive = nullptr;
	}
}

void UKMInteractiveInstance::SetDirectionVisual(float direction, bool bForceRotate, USkeletalMeshComponent* otherSkeletalMeshComp)
{
}

void UKMInteractiveInstance::SetDirection(float direction, bool bForceRotate)
{
	if (IsDead())
	{
		return;
	}

	if (HasGameplayTag(FKMGameplayTagName::Block_Control_Rotation_Tag))
	{
		return;
	}

	if (FMath::IsNearlyEqual(Direction, direction) && !bForceRotate)
	{
		return;
	}

	Direction = direction;
	
	if (OwnerActor.IsValid())
	{
		float angle = Direction * PI * 2.f;
		FVector newForwardVector(FMath::Cos(angle), FMath::Sin(angle),0.f);

		FRotator newRotation = newForwardVector.Rotation();
		OwnerActor->SetActorRotation(newRotation);
		OwnerActor->GetRootComponent()->UpdateComponentToWorld();

		SetDirectionVisual(direction, bForceRotate);
	}
}