#include "KMGameModeCharacterSelect.h"

#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"

void AKMGameModeCharacterSelect::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeCharacterSelect::SelectCharacter(AController* newPlayer, const FName& newCharacterId)
{
	if (newCharacterId == NAME_None)
	{
		return;
	}

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(gameObjectSubsystem);

	gameObjectSubsystem->RemoveAllGameObjects();
	
	HeroId = newCharacterId;
	RestartPlayer(newPlayer);
}

void AKMGameModeCharacterSelect::RestartPlayer(AController* newPlayer)
{
	Super::RestartPlayer(newPlayer);
}

void AKMGameModeCharacterSelect::OnSpawnCharacterInstance_Implementation(UKMHeroInstance* newHeroInstance)
{
	Super::OnSpawnCharacterInstance_Implementation(newHeroInstance);

	if (IsValid(newHeroInstance))
	{
		newHeroInstance->OnCharacterSelected();
	}
}