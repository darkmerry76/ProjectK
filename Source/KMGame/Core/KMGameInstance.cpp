#include "KMGameInstance.h"

#include "EMMartialArtsModule.h"
#include "Actor/KMCameraActor.h"
#include "Components/Viewport.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameObject/KMCharacterInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sequencer/EMCameraCacheManager.h"
#include "System/KMUiSubsystem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMGameInstance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMGameInstance::UKMGameInstance() : Super()
{
	bIsInitLevel = false;
}

UKMGameInstance* UKMGameInstance::GetGameInstance(const UObject* worldContextObject)
{
	check(IsValid(worldContextObject) == true);
	
	UWorld* world = worldContextObject->GetWorld();
	check(IsValid(world) == true);
	UKMGameInstance* gameInstance = Cast<UKMGameInstance>(world->GetGameInstance());
	check(IsValid(gameInstance) == true);

	return gameInstance;
}

void UKMGameInstance::Init()
{
	Super::Init();

	CameraCacheManager = MakeShared<FEMCameraCacheManager>();
	CameraCacheManager->Initialize();

	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &UKMGameInstance::OnViewportResized);

	UKMCharacterInstance::DefaultPassiveSkills.Empty();
	for (FKMSkillKeyBase skillKey : DefaultPassiveSkills)
	{
		UKMCharacterInstance::DefaultPassiveSkills.Emplace(FKMSkillKey(skillKey));
	}
}

void UKMGameInstance::OpenInitLevel(FSoftObjectPath mapPath)
{
	UGameplayStatics::OpenLevel(this, *mapPath.GetAssetName());
	bIsInitLevel = true;
}

const UKMAbilityEffectSet* UKMGameInstance::GetAnormalAbilitySet() const
{
	return AnormalAbilitySet;
}

TSharedPtr<FEMCameraCacheManager> UKMGameInstance::GetCameraCacheManager() const
{
	return CameraCacheManager;
}

void UKMGameInstance::Shutdown()
{
	Super::Shutdown();

	GEngine->GameViewport->Viewport->ViewportResizedEvent.RemoveAll(this);
}

void UKMGameInstance::SetFixedCamera(AKMCameraActor* newCameraActor)
{
	CameraActor = newCameraActor;
}

void UKMGameInstance::OnViewportResized(FViewport* viewport, uint32 Unused)
{
	if (viewport == nullptr || IsValid(CameraActor.Get()) == false)
	{
		return;
	}

	CameraActor->SetViewportOnAspectRatio(viewport);
}

UKMPlayerAccount* UKMGameInstance::GetPlayerAccount() const
{
	return PlayerAccount;
}

void UKMGameInstance::OnViewportCreated()
{
	Super::OnViewportCreated();

	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->CreateRoot();
	}
}