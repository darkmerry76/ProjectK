#include "EMGameObjectSubsystem.h"
#include "GameObject/EMGameObjectInstance.h"

void UEMGameObjectSubsystem::Initialize()
{
	Super::Initialize();
}

void UEMGameObjectSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEMGameObjectSubsystem::OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources)
{
	Super::OnWorldCleanup(cleaupWorld, bSessionEnded, bCleanupResources);

	RemoveAllGameObjects();
}

int32 UEMGameObjectSubsystem::AddGameObject(UEMGameObjectInstance* newGameObjectInstance)
{
	int32 newId = LastetId++;
	
	GameObjectMap.Emplace(newId, newGameObjectInstance);
	newGameObjectInstance->SetId(newId);
	newGameObjectInstance->BeginPlay();
	
	return newId;
}

UEMGameObjectInstance* UEMGameObjectSubsystem::GetGameObject(int32 gameObjectId)
{
	TObjectPtr<UEMGameObjectInstance>* resultObject = GameObjectMap.Find(gameObjectId);
	if (resultObject == nullptr || (*resultObject) == nullptr)
	{
		return nullptr;
	}
	
	return *resultObject;
}

bool UEMGameObjectSubsystem::HasGameObject(int32 gameObjectId) const
{
	return GameObjectMap.Contains(gameObjectId);
}

bool UEMGameObjectSubsystem::RemoveGameObject(int32 gameObjectId)
{
	TObjectPtr<UEMGameObjectInstance> gameObjectInstance = GetGameObject(gameObjectId);
	if (IsValid(gameObjectInstance) == false)
	{
		return false;
	}

	gameObjectInstance->EndPlay();
	GameObjectMap.Remove(gameObjectId);
	
	RemoveGameObjectDelegate.Broadcast(gameObjectInstance);

	return true;
}

void UEMGameObjectSubsystem::RemoveAllGameObjects(const TArray<TSubclassOf<UEMGameObjectInstance>>& ignoreInstanceClasses)
{
	for (auto objectItr = GameObjectMap.CreateIterator(); objectItr; ++objectItr)
	{
		bool isIgnoreClass = false;
		if (!ignoreInstanceClasses.IsEmpty())
		{
			for (auto classItr : ignoreInstanceClasses)
			{
				if (objectItr.Value()->IsA(classItr) == true)
				{
					isIgnoreClass = true;
					break;
				}
			}
		}

		if (isIgnoreClass == false)
		{
			objectItr.Value()->EndPlay();
			objectItr.RemoveCurrent();
		}
	}
}