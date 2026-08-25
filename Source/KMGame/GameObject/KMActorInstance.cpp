#include "KMActorInstance.h"
#include "GameFramework/Actor.h"

UKMActorInstance::UKMActorInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMActorInstance::BeginPlay()
{
	Super::BeginPlay();
}

void UKMActorInstance::EndPlay()
{
	Super::EndPlay();

	if (IsValid(Actor))
	{
		ClearActor();
	}
}

void UKMActorInstance::SetActor(AActor* newActor)
{
	ClearActor();

	Actor = newActor;
	if (IsValid(Actor))
	{
		Actor->OnDestroyed.AddDynamic(this, &ThisClass::OnActorDestroyed);
	}
}

void UKMActorInstance::ClearActor()
{
	if (IsValid(Actor))
	{
		Actor->OnDestroyed.RemoveAll(this);
		Actor->Destroy();
	}
	
	Actor = nullptr;
}

AActor* UKMActorInstance::GetActor() const
{
	return Actor;
}

void UKMActorInstance::OnActorDestroyed(AActor* destroyedActor)
{
	check(IsValid(destroyedActor) && destroyedActor == Actor);

	ActorInstanceDestroyedDelegate.ExecuteIfBound(this);
}