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

bool UKMInteractiveInstance::SetDirection(float direction, bool bForceRotate)
{
	return Super::SetDirection(direction, bForceRotate);
}