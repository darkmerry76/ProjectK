#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "EMTableType.h"
#include "EMAssetManager.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;
	
	UObject* GetAsset(const TCHAR* assetId);
	UObject* GetAsset(const FEMPrimaryAssetRef& pdaRef);
	UObject* GetAsset(const FName& assetId);
	UObject* GetAsset(const FString& assetId);
	UObject* GetAsset(const FPrimaryAssetId& assetId);
	
	bool RemoveAssetByString(const FString& stringId);
	virtual bool RemoveAssetById(const FPrimaryAssetId& assetId);

	void RemoveAllAssets();

protected:
	virtual int32 ScanPathsForPrimaryAssets(FPrimaryAssetType PrimaryAssetType, const TArray<FString>& Paths, UClass* BaseClass, bool bHasBlueprintClasses, bool bIsEditorOnly, bool bForceSynchronousScan) override;
	bool IsBlueprintChildOf(const UBlueprint* Blueprint, const UClass* ParentClass) const;

protected:
	UPROPERTY()
	TMap<FPrimaryAssetId, TObjectPtr<UObject>> Assets;
};