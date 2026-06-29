#include "KMWorldSubsystem.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMWorldSubsystem* UKMWorldSubsystem::GetWorldSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMWorldSubsystem>(UEMGameInstanceSubsystem::GetSubsystem(worldContextObject, UKMWorldSubsystem::StaticClass()));
}

void UKMWorldSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMWorldSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
	Super::OnLevelAdded(level, world);
}