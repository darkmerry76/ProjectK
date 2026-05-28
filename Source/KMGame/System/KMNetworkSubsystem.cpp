#include "KMNetworkSubsystem.h"
#include "KMBattleSubsystem.h"

UKMNetworkSubsystem* UKMNetworkSubsystem::GetNetworkSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMNetworkSubsystem>(GetSubsystem(worldContextObject, UKMNetworkSubsystem::StaticClass()));
}

void UKMNetworkSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMNetworkSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FGuid& UKMNetworkSubsystem::GetAuthGuid() const
{
	return authAuthGuid;
}