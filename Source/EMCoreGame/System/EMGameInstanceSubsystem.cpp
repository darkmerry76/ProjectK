#include "EMGameInstanceSubsystem.h"
#include "Engine/GameInstance.h"

UEMGameInstanceSubsystem* UEMGameInstanceSubsystem::GetSubsystem(const UObject* worldContextObject, TSubclassOf<UGameInstanceSubsystem> subsystemClass)
{
	check(IsValid(worldContextObject));
	
	UWorld* world = worldContextObject->GetWorld();
	check(IsValid(world));
	UGameInstance* instance = world->GetGameInstance();
	check(IsValid(instance));

	return Cast<UEMGameInstanceSubsystem>(world->GetGameInstance()->GetSubsystemBase(subsystemClass));
}

void UEMGameInstanceSubsystem::Initialize()
{
	FWorldDelegates::OnPreWorldInitialization.AddUObject(this, &ThisClass::OnPreWorldInitialization);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::OnPostWorldInitialization);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::OnWorldCleanup);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMapWithWorld);

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

}

void UEMGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UEMGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEMGameInstanceSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
}

void UEMGameInstanceSubsystem::OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
}

void UEMGameInstanceSubsystem::OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
}

void UEMGameInstanceSubsystem::OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources)
{

}

void UEMGameInstanceSubsystem::OnPostLoadMapWithWorld(UWorld* loadedWorld)
{
}
