#include "KMCameraActorBase.h"

AKMCameraActorBase::AKMCameraActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMCameraActorBase::SetTargetActor(AActor* newTargetActor)
{
	TargetActor = newTargetActor;
}

AActor* AKMCameraActorBase::GetTargetActor() const
{
	if (!TargetActor.IsValid())
	{
		return nullptr;
	}
	return TargetActor.Get();
}

void AKMCameraActorBase::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}