#include "EMAssetManager.h"

#include "EMTableType.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"

void UEMAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UObject* UEMAssetManager::GetAsset(const TCHAR* assetId)
{
	return GetAsset(FString(assetId));	
}

UObject* UEMAssetManager::GetAsset(const FEMPrimaryAssetRef& pdaRef)
{
	return GetAsset(pdaRef.PdaKey); 
}

UObject* UEMAssetManager::GetAsset(const FString& assetId)
{
	return GetAsset(FPrimaryAssetId::FromString(assetId));
}

UObject* UEMAssetManager::GetAsset(const FName& assetId)
{
	return GetAsset(assetId.ToString());
}

UObject* UEMAssetManager::GetAsset(const FPrimaryAssetId& assetId)
{
	if (assetId.IsValid() == false)
	{
		return nullptr;
	}
	
	TObjectPtr<UObject>* resultAsset = Assets.Find(assetId);
	if (resultAsset != nullptr && (*resultAsset) != nullptr)
	{
		return *resultAsset;
	}

	FSoftObjectPath path = 
		UAssetManager::Get().GetPrimaryAssetPath(assetId);
	
	TSharedPtr<FStreamableHandle> handle = LoadPrimaryAsset(assetId, TArray<FName>(), nullptr);
	if (handle.IsValid() == true)
	{
		handle->WaitUntilComplete();
	}

	UObject* result = path.TryLoad();
	UObject* newObject = GetPrimaryAssetObject(assetId);
	if(::IsValid(newObject) == false)
	{
		
		return nullptr;
	}
	
	Assets.Emplace(assetId, newObject);
	return newObject;
}

bool UEMAssetManager::RemoveAssetByString(const FString& stringId)
{
	return RemoveAssetById(FPrimaryAssetId::FromString(stringId));
}

bool UEMAssetManager::RemoveAssetById(const FPrimaryAssetId& assetId)
{
	if (Assets.Contains(assetId) == false)
	{
		return false;
	}
	Assets.Remove(assetId);
	return true;
}

void UEMAssetManager::RemoveAllAssets()
{
	Assets.Empty();
}

int32 UEMAssetManager::ScanPathsForPrimaryAssets(FPrimaryAssetType PrimaryAssetType, const TArray<FString>& Paths, UClass* BaseClass, bool bHasBlueprintClasses, bool bIsEditorOnly, bool bForceSynchronousScan)
{
	int32 result = Super::ScanPathsForPrimaryAssets(PrimaryAssetType, Paths, BaseClass, bHasBlueprintClasses, bIsEditorOnly);
	return result;
}

bool UEMAssetManager::IsBlueprintChildOf(const UBlueprint* blueprint, const UClass* parentClass) const
{
/*	if (!::IsValid(blueprint) || !::IsValid(parentClass))
	{
		return false;
	}

	const UClass* blueprintClass = blueprint->GeneratedClass;
	while(::IsValid(blueprintClass))
	{
		if (blueprintClass->IsChildOf(parentClass))
		{
			return true;
		}
		
		const UBlueprintGeneratedClass* generatedblueprintClass = Cast<UBlueprintGeneratedClass>(blueprintClass);
		if (::IsValid(generatedblueprintClass) && ::IsValid(generatedblueprintClass->ClassGeneratedBy))
		{
			const UBlueprint* parentBlueprint = Cast<UBlueprint>(generatedblueprintClass->ClassGeneratedBy);
			if (::IsValid(parentBlueprint))
			{
				if (blueprintClass == parentBlueprint->GeneratedClass)
				{
					return true;
				}
				else
				{
					blueprintClass = parentBlueprint->GeneratedClass;
					continue;
				}
			}
		}
		break;
	}*/

	return false;
}
