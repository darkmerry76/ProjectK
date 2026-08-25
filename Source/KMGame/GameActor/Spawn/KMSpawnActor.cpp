#include "KMSpawnActor.h"
#include "System/KMGameObjectSubsystem.h"

AKMSpawnCharacter::AKMSpawnCharacter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnCharacter::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(gameObjectSubsystem);

	UKMCharacterInstance* characterInstance = gameObjectSubsystem->SpawnCharacterObject(CharacterId, GetActorTransform());
	check(characterInstance);
}

AKMSpawnInteractive::AKMSpawnInteractive(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnInteractive::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(gameObjectSubsystem);

	UKMInteractiveInstance* interactiveInstance = gameObjectSubsystem->SpawnInteractiveObject(InteractiveId, GetActorTransform());
	check(interactiveInstance);
}