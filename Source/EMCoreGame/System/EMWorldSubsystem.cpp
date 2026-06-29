#include "EMWorldSubsystem.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UEMWorldSubsystem::Initialize()
{
	Super::Initialize();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);
}

void UEMWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
}

void UEMWorldSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
	if (world != GetWorld())
	{
		return;
	}
}

void UEMWorldSubsystem::OnLoadingComplete()
{
	
}