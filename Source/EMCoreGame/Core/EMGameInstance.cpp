#include "EMGameInstance.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "System/EMGameInstanceSubsystem.h"
#include "UObject/UObjectIterator.h"

UEMGameInstance::UEMGameInstance() : Super()
{
}

void UEMGameInstance::Init()
{
	LoadBlueprintSubsystem();
	
	Super::Init();

	const TArray<UGameInstanceSubsystem*>& subsystemArray = GetSubsystemArrayCopy<UGameInstanceSubsystem>();
	for (auto subsystem : subsystemArray)
	{
		if (UEMGameInstanceSubsystem* gameInstanceSubsystem = Cast<UEMGameInstanceSubsystem>(subsystem))
		{
			gameInstanceSubsystem->Initialize();
			gameInstanceSubsystem->OnInitalize();
		}
	}

	OnPostInitalize();
}

void UEMGameInstance::LoadBlueprintSubsystem()
{
	TArray<FAssetData> allAssets;
	IAssetRegistry::Get()->GetAllAssets(allAssets);
	
	struct FClassAssetElement
	{
		FClassAssetElement(UClass* gameInstanceSubclass) : Class(gameInstanceSubclass) { }
		FClassAssetElement(const FAssetData& assetData) : AssetData(assetData), Class(nullptr) { }

		bool IsAsset() const { return !IsValid(Class); };
		
		FAssetData AssetData;
		UClass* Class;
	};

	TMap<FSoftClassPath, TSharedPtr<FClassAssetElement>> assetDataMap;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* gameInstanceSubsystemClass = *It;
		if (IsValid(gameInstanceSubsystemClass->GetDefaultObject()) &&
			gameInstanceSubsystemClass->GetDefaultObject()->IsA<UGameInstanceSubsystem>())
		{
			TSharedPtr<FClassAssetElement> newClassAssetElement = MakeShared<FClassAssetElement>(gameInstanceSubsystemClass);
			assetDataMap.Emplace(gameInstanceSubsystemClass->GetPathName(), newClassAssetElement);
		}
	}
	
	TArray<TSharedPtr<FClassAssetElement>> contentAssets;
	for (const FAssetData& asset : allAssets)
	{
		if (!asset.PackageName.ToString().StartsWith(TEXT("/Game/")))
		{
			continue;
		}
		
		FString finalObjectPath = asset.PackageName.ToString().Replace(TEXT(".Default__"), TEXT("."));

		TSharedPtr<FClassAssetElement> newClassAssetElement = MakeShared<FClassAssetElement>(asset);
		assetDataMap.Add(finalObjectPath, newClassAssetElement);
		contentAssets.Emplace(newClassAssetElement);
	}
	
	auto isChildOfTarget = [&](const TSharedPtr<FClassAssetElement>& classAssetElement, const FSoftClassPath& targetClassPath, auto&& recursiveRef) -> bool
	{
		FString parentClassPath;
		
		if (classAssetElement->IsAsset())
		{
			classAssetElement->AssetData.GetTagValue("ParentClass", parentClassPath);
		}
		else
		{
			parentClassPath = classAssetElement->Class->GetSuperClass()->GetPathName();
		}
		
		FSoftClassPath parentSoftPath(parentClassPath);
		if (parentSoftPath == targetClassPath)
		{
			return true;
		}
		
		if (assetDataMap.Contains(parentSoftPath))
		{
			return recursiveRef(assetDataMap[parentSoftPath], targetClassPath, recursiveRef);
		}
		return false;
	};

	FSoftClassPath targetClassPath = UGameInstanceSubsystem::StaticClass()->GetPathName();
	for (const TSharedPtr<FClassAssetElement>& classAssetElement : contentAssets)
	{
		if (isChildOfTarget(classAssetElement, targetClassPath, isChildOfTarget))
		{
			if (classAssetElement->IsAsset())
			{
				classAssetElement->AssetData.ToSoftObjectPath().TryLoad();
			}
		}
	}
}

void UEMGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UEMGameInstance::OnMinimumTimeEnd()
{
	
}
