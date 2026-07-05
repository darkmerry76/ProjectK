#include "KMGameModeCharacterSelect.h"
#include "Character/KMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "Tables/Generated/KMTable_Character.h"

void AKMGameModeCharacterSelect::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeCharacterSelect::SelectCharacter(AController* newPlayer, const FName& newCharacterId, bool bForce)
{
	if (newCharacterId == NAME_None)
	{
		return;
	}

	if (!bForce && (LatestHeroInstance.IsValid() && LatestHeroInstance->GetTable()->Id == newCharacterId))
	{
		return;
	}

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(gameObjectSubsystem);

	gameObjectSubsystem->RemoveAllGameObjects();
	
	HeroId = newCharacterId;
	RestartPlayer(newPlayer);

	HeroSelectDelegate.Broadcast(newCharacterId);
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
		if (AKMCharacter* character = newHeroInstance->GetCharacter())
		{
			character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		}
		newHeroInstance->OnHeroSelected();
	}

	LatestHeroInstance = newHeroInstance;
}

void AKMGameModeCharacterSelect::OnWorldLoadingComplete_Implementation()
{
	Super::OnWorldLoadingComplete_Implementation();
	SelectCharacter(GetWorld()->GetFirstPlayerController(), DefaultSelectTableId, true);
}

void AKMGameModeCharacterSelect::OnEnterGame_Implementation()
{
	if (LatestHeroInstance.IsValid())
	{
		LatestHeroInstance->OnEnterGame();
	}
}