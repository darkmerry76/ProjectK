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
}

void UEMGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}