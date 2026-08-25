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

void UKMInteractiveInstance::SetInteractiveActor(AKMInteractiveActorBase* newInteractiveActor)
{
	Interactive = newInteractiveActor;
}

AKMInteractiveActorBase* UKMInteractiveInstance::GetInteractiveActor() const
{
	if (!Interactive.IsValid())
	{
		return nullptr;
	}
	return Interactive.Get();
}