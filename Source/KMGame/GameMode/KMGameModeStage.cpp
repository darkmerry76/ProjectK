#include "KMGameModeStage.h"
#include "Character/KMCharacter.h"
#include "GameObject/KMHeroInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMNarrativeSubsystem.h"

AKMGameModeStage::AKMGameModeStage() : Super()
{
	bIsAutoAttachStateWidget = false;
}

void AKMGameModeStage::BeginPlay()
{
	Super::BeginPlay();

	if (UKMNarrativeSubsystem* narrativeSubsystem = UKMNarrativeSubsystem::GetNarrativeSubsystem(this))
	{
		narrativeSubsystem->EventDelegate.AddUObject(this, &ThisClass::OnNarrativeEventListening);
	}
}

void AKMGameModeStage::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (UKMNarrativeSubsystem* narrativeSubsystem = UKMNarrativeSubsystem::GetNarrativeSubsystem(this))
	{
		narrativeSubsystem->EventDelegate.RemoveAll(this);
	}
}

void AKMGameModeStage::RestartPlayer(AController* newPlayer)
{
	Super::RestartPlayer(newPlayer);

	if (HeroId != NAME_None)
	{
		UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
		check(gameObjectSubsystem);

		if (UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance())
		{
			gameObjectSubsystem->RemoveGameObject(authCharacterInstance->GetId());
		}

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
}

void AKMGameModeStage::OnNarrativeEventListening(FGameplayTag newTag)
{
	if (newTag == FKMGameplayTagName::Game_Event_Stage_Begin)
	{
		SpawnStateWidget();
	}
}
