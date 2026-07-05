#include "KMGameModeStage.h"
#include "Character/KMCharacter.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"

void AKMGameModeStage::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeStage::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void AKMGameModeStage::RestartPlayer(AController* newPlayer)
{
	Super::RestartPlayer(newPlayer);

	if (HeroId != NAME_None)
	{
		UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
		check(gameObjectSubsystem);

		UKMHeroInstance* heroInstance = Cast<UKMHeroInstance>(gameObjectSubsystem->SpawnCharacterObject(HeroId, HeroSpwnTransform));
		check(IsValid(heroInstance));
		newPlayer->Possess(heroInstance->GetCharacter());
		OnSpawnCharacterInstance(heroInstance);
	}
}

void AKMGameModeStage::OnSpawnCharacterInstance_Implementation(UKMHeroInstance* newHeroInstance)
{
}