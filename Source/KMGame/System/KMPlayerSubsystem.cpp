#include "KMPlayerSubsystem.h"
#include "KMNetworkSubsystem.h"

UKMPlayerSubsystem* UKMPlayerSubsystem::GetPlayerSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMPlayerSubsystem>(GetSubsystem(worldContextObject, UKMPlayerSubsystem::StaticClass()));
}

void UKMPlayerSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMPlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}