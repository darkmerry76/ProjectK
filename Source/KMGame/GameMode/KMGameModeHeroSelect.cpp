#include "KMGameModeHeroSelect.h"
#include "Components/CapsuleComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "Tables/Generated/KMTable_Character.h"

void AKMGameModeHeroSelect::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeHeroSelect::SelectHero(AController* newPlayer, const FName& newCharacterId, bool bForce)
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

void AKMGameModeHeroSelect::RestartPlayer(AController* newPlayer)
{
	Super::RestartPlayer(newPlayer);

	if (HeroId != NAME_None)
	{
		UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
		check(gameObjectSubsystem);

		UKMHeroInstance* heroInstance = Cast<UKMHeroInstance>(gameObjectSubsystem->SpawnCharacterObject(HeroId, FTransform::Identity));
		check(IsValid(heroInstance));
		newPlayer->Possess(heroInstance->GetCharacter());
		OnSpawnHeroInstance(heroInstance);
	}
}

void AKMGameModeHeroSelect::OnSpawnHeroInstance_Implementation(UKMHeroInstance* newHeroInstance)
{
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

void AKMGameModeHeroSelect::OnWorldLoadingComplete_Implementation()
{
	Super::OnWorldLoadingComplete_Implementation();
	if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
	{
		SelectHero(playerController, DefaultSelectTableId, true);
	}
}

void AKMGameModeHeroSelect::OnEnterGame_Implementation()
{
	if (LatestHeroInstance.IsValid())
	{
		LatestHeroInstance->OnEnterGame();
	}
}