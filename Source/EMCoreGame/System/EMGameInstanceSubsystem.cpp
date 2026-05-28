#include "EMGameInstanceSubsystem.h"
#include "Engine/GameInstance.h"

UEMGameInstanceSubsystem* UEMGameInstanceSubsystem::GetSubsystem(const UObject* worldContextObject, TSubclassOf<UGameInstanceSubsystem> subsystemClass)
{
	check(IsValid(worldContextObject) == true);
	
	UWorld* world = worldContextObject->GetWorld();
	check(IsValid(world) == true);
	UGameInstance* instance = world->GetGameInstance();
	check(IsValid(instance) == true);

	return Cast<UEMGameInstanceSubsystem>(world->GetGameInstance()->GetSubsystemBase(subsystemClass));
}

void UEMGameInstanceSubsystem::Initialize()
{
}

void UEMGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::OnPostWorldInitialization);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMapWithWorld);
}

void UEMGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UEMGameInstanceSubsystem::OnPostWorldInitialization(UWorld* loadedWorld, const UWorld::InitializationValues iVS)
{
	
}

void UEMGameInstanceSubsystem::OnPostLoadMapWithWorld(UWorld* loadedWorld)
{
	
}