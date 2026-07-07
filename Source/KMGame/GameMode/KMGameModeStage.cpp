#include "KMGameModeStage.h"
#include "Character/KMCharacter.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMUiSubsystem.h"

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
		OnSpawnHeroInstance(heroInstance);
	}
}

void AKMGameModeStage::OnSpawnHeroInstance_Implementation(UKMHeroInstance* newHeroInstance)
{
}

void AKMGameModeStage::OnWorldLoadingComplete_Implementation()
{
	Super::OnWorldLoadingComplete_Implementation();

	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->CreateRoot();
	}
}
