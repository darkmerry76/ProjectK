#include "KMSpawnActor.h"
#include "System/KMGameObjectSubsystem.h"

AKMSpawnActor::AKMSpawnActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnActor::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(gameObjectSubsystem);

	UKMCharacterInstance* characterInstance = gameObjectSubsystem->SpawnCharacterObject(CharacterId, GetActorTransform());
	check(characterInstance);
}