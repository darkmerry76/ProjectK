#include "KMInteractiveActorBase.h"

#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"

AKMInteractiveActorBase::AKMInteractiveActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootScene")));
	
	CurveWarping  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));
}

UKMMartialArtsComponent* AKMInteractiveActorBase::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
}

UKMCurveWarpingComponent* AKMInteractiveActorBase::GetCurveWarping() const
{
	return CurveWarping;
}

UKMInteractiveInstance* AKMInteractiveActorBase::GetInteractiveInstance() const
{
	if (!InteractiveInstance.IsValid())
	{
		return nullptr;
	}
	return InteractiveInstance.Get();
}

UKMGameObjectInstance* AKMInteractiveActorBase::GetGameObjectInstance() const
{
	if (!InteractiveInstance.IsValid())
	{
		return nullptr;
	}
	return InteractiveInstance.Get();
};

void AKMInteractiveActorBase::PossessedByGameObjectInstance(UKMGameObjectInstance* newGameObjectInstance)
{
	InteractiveInstance = Cast<UKMInteractiveInstance>(newGameObjectInstance);
	check(InteractiveInstance.IsValid());
	InteractiveInstance->SetOwnerActor(this);
}