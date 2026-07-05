#include "KMWorldSubsystem.h"

#include "Core/KMGameViewportClient.h"
#include "Core/KMWorldSettings.h"
#include "GameMode/KMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Util/KMUtil.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMWorldSubsystem* UKMWorldSubsystem::GetWorldSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMWorldSubsystem>(UEMGameInstanceSubsystem::GetSubsystem(worldContextObject, UKMWorldSubsystem::StaticClass()));
}

void UKMWorldSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMWorldSubsystem::OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	Super::OnPreWorldInitialization(newWorld, iVS);
}

void UKMWorldSubsystem::OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	Super::OnPostWorldInitialization(newWorld, iVS);

	if (AKMWorldSettings* worldSettings = Cast<AKMWorldSettings>(newWorld->GetWorldSettings()))
	{
		if (worldSettings->bIsShowLoadingScreen)
		{
			newWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
			{
				UKMUtil::PlayLoadingScreen(this);	
			}));
		}
	}
}

void UKMWorldSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
	Super::OnLevelAdded(level, world);
}

void UKMWorldSubsystem::OnLoadingComplete()
{
	bool bShowLoadingScreen = false;
	if (AKMWorldSettings* worldSettings = Cast<AKMWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		bShowLoadingScreen = worldSettings->bIsShowLoadingScreen; 
	}
	if (bShowLoadingScreen)
	{
		UKMUtil::StopLoadingScreen(this, FKMLoadingScreenCompleteDelegate::CreateLambda([this]()
		{
			UKMUtil::PlaySlateFade(this,1.f, 0.f, 1.f);
			if (AKMGameModeBase* gameMode = Cast<AKMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				gameMode->OnWorldLoadingComplete();
			}
		}), 2.f);
	}
	else
	{
		UKMUtil::PlaySlateFade(this,1.f, 0.f, 1.f);
		if (AKMGameModeBase* gameMode = Cast<AKMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			gameMode->OnWorldLoadingComplete();
		}
	}
	Super::OnLoadingComplete();
}