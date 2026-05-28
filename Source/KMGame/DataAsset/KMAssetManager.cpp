#include "KMAssetManager.h"
#include "Engine/Engine.h"

UKMAssetManager* UKMAssetManager::GetAssetManager()
{
	return Cast<UKMAssetManager>(GEngine->AssetManager);	
}

void UKMAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

