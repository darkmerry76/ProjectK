#include "EMWorldSubsystem.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UEMWorldSubsystem::Initialize()
{
	Super::Initialize();
}

void UEMWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

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
}

void UEMWorldSubsystem::OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	LoadingState = EEMWorldLoadingState::Ready;
}

void UEMWorldSubsystem::OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	LoadingState = EEMWorldLoadingState::Loading;

	newWorld->OnAllLevelsChanged().AddUObject(this, &ThisClass::OnAllLevelsChanged);
}

void UEMWorldSubsystem::OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources)
{
	
}

void UEMWorldSubsystem::OnPostLoadMapWithWorld(UWorld* loadedWorld)
{
}

void UEMWorldSubsystem::OnAllLevelsChanged()
{
	if (LoadingState == EEMWorldLoadingState::Complete)
	{
		return;
	}
	
	if (IsAllStreamingLevelLoaded())
	{
		LoadingState = EEMWorldLoadingState::Complete;
		OnLoadingComplete();
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
	LoadingCompleteDelegate.Broadcast();
}