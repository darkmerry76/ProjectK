#include "EMWorldSubsystem.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UEMWorldSubsystem::Initialize()
{
	Super::Initialize();

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UEMWorldSubsystem::OnPostWorldInitialization);
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);
}

void UEMWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
}

EEMWorldLoadingState UEMWorldSubsystem::GetLoadingState() const
{
	return LoadingState;
}

void UEMWorldSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
	if (world != GetWorld())
	{
		return;
	}
	LoadingState = EEMWorldLoadingState::Loading;
}

void UEMWorldSubsystem::OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	LoadingState = EEMWorldLoadingState::Ready;

	newWorld->OnAllLevelsChanged().AddUObject(this, &ThisClass::OnAllLevelsChanged);
}

void UEMWorldSubsystem::OnAllLevelsChanged()
{
	if (LoadingState != EEMWorldLoadingState::Loading)
	{
		return;
	}
	
	if (IsAllStreamingLevelLoaded())
	{
		LoadingState = EEMWorldLoadingState::Complete;
	}
}

bool UEMWorldSubsystem::IsAllStreamingLevelLoaded() const
{
	UWorld* world = GetWorld();

	if (world->HasStreamingLevelsToConsider())
	{
		return false;
	}

	for (ULevelStreaming* streaming : world->GetStreamingLevels())
	{
		if (!IsValid(streaming))
		{
			continue;
		}

		if (streaming->ShouldBeLoaded())
		{
			if (!streaming->IsLevelLoaded())
			{
				return false;
			}
			if (streaming->ShouldBeVisible() && !streaming->IsLevelVisible())
			{
				return false;
			}
		}
	}

	return true;
}

void UEMWorldSubsystem::OnLoadingComplete()
{
	
}