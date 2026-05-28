#pragma once

#include "CoreMinimal.h"
#include "DataAsset/EMAssetManager.h"
#include "KMAssetManager.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAssetManager : public UEMAssetManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static UKMAssetManager* GetAssetManager();
	
	virtual void StartInitialLoading() override;
	
};