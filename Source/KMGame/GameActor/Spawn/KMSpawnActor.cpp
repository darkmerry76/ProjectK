#include "KMSpawnActor.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "System/KMGameObjectSubsystem.h"

AKMSpawnCharacter::AKMSpawnCharacter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnCharacter::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMCharacterInstance* characterInstance = gameObjectSubsystem->SpawnCharacterObject(CharacterId, GetActorTransform());
	check(IsValid(characterInstance));
}

AKMSpawnInteractive::AKMSpawnInteractive(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnInteractive::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMInteractiveInstance* interactiveInstance = gameObjectSubsystem->SpawnInteractiveObject(InteractiveId, GetActorTransform());
	check(IsValid(interactiveInstance));
}