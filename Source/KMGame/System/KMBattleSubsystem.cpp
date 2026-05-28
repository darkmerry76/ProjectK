#include "KMBattleSubsystem.h"
#include "KMGameObjectSubsystem.h"
#include "KMPlayerSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMMonsterInstance.h"
#include "Core/KMDefine.h"
#include "Engine/Level.h"

UKMBattleSubsystem* UKMBattleSubsystem::GetBattleSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMBattleSubsystem>(GetSubsystem(worldContextObject, UKMBattleSubsystem::StaticClass()));
}

UKMBattleSubsystem::UKMBattleSubsystem() : Super()
{
}

void UKMBattleSubsystem::Initialize()
{
	Super::Initialize();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem) == true);
	gameObjectSubsystem->RemoveGameObjectDelegate.AddUObject(this, &UKMBattleSubsystem::OnRemoveGameObject);

	bIsBattleClear = false;
}

void UKMBattleSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	
	if(IsValid(gameObjectSubsystem) == true)
	{
		gameObjectSubsystem->RemoveGameObjectDelegate.RemoveAll(this);
	}
}

void UKMBattleSubsystem::OnPostLoadMapWithWorld(UWorld* loadedWorld)
{
}

void UKMBattleSubsystem::OnRemoveGameObject(UEMGameObjectInstance* gameObjectInstance)
{
	UKMMonsterInstance* monsterInstance = Cast<UKMMonsterInstance>(gameObjectInstance);
	if (IsValid(monsterInstance) == false)
	{
		return;
	}

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem) == true);

	if (gameObjectSubsystem->NumMonster() <= 0)
	{
		ClearBattleState();
	}
}

void UKMBattleSubsystem::OnSubLevelShown()
{
}

void UKMBattleSubsystem::ChangeBattleState(EKMBattleState nextState)
{
	BattleStateChangedDelegate.Broadcast(BattleState, nextState);
	BattleState = nextState;
}

void UKMBattleSubsystem::StartBattleState()
{
	ChangeBattleState(EKMBattleState::Playing);
	bIsBattleClear = false;
}

void UKMBattleSubsystem::ClearBattleState()
{
	ChangeBattleState(EKMBattleState::Clear);
	bIsBattleClear = true;
}

void UKMBattleSubsystem::EndBattleState()
{
	ChangeBattleState(EKMBattleState::End);
}

void UKMBattleSubsystem::ClearStage()
{
	RemoveCurrentLevel();
}

bool UKMBattleSubsystem::IsBattleClear() const
{
	return bIsBattleClear;
}

void UKMBattleSubsystem::RemoveCurrentLevel()
{
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem) == true);
	gameObjectSubsystem->RemoveAllGameObjects();

	if (IsValid(CurrentLevel) == true && IsValid(CurrentLevel->GetLoadedLevel()) == true)
	{
		CurrentLevel->SetShouldBeVisible(false);
		CurrentLevel->SetShouldBeLoaded(false);
		CurrentLevel->SetIsRequestingUnloadAndRemoval(true);

		GetWorld()->FlushLevelStreaming(EFlushLevelStreamingType::Full);
		GetWorld()->RemoveStreamingLevel(CurrentLevel);
	}
	CurrentLevel = nullptr;
}