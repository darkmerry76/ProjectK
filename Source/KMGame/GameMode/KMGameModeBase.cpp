#include "KMGameModeBase.h"

#include "Character/KMCharacter.h"
#include "Core/KMGameInstance.h"
#include "Engine/World.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMUiSubsystem.h"

void AKMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UKMUiSubsystem::GetUiSubsystem(this)->Activate();
	UKMGameInstance* gameInstance = Cast<UKMGameInstance>(GetWorld()->GetGameInstance());
	check(IsValid(gameInstance) == true);
	
	if (gameInstance->IsInitLevel() == false && InitMap.IsValid() == true)
	{
		gameInstance->OpenInitLevel(InitMap);
	}
}

bool AKMGameModeBase::IsInitMap(const UWorld* otherWorld) const
{
	if (IsValid(otherWorld) == false)
	{
		return false;
	}
	return otherWorld->GetFName() == *InitMap.GetAssetName();
}

void AKMGameModeBase::RestartPlayer(AController* newPlayer)
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

void AKMGameModeBase::OnSpawnCharacterInstance_Implementation(UKMHeroInstance* newHeroInstance)
{
}

void AKMGameModeBase::OnWorldLoadingComplete_Implementation()
{
}